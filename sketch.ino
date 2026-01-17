#include <MD_MAX72xx.h>
#include <SPI.h>
#include <IRremote.h>
#include <vector>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 16

#define CS_PIN    27
#define DATA_PIN  23
#define CLK_PIN   18
#define IR_PIN    26

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define SCREEN_WIDTH  32
#define SCREEN_HEIGHT 32

int grosimePerete = 3;
byte maze[32][32];

int destX = -1, destY = -1;
int playerX = -1, playerY = -1;
bool destVertical = false;

bool gameOver = false;

struct Point { int x; int y; };

void setGlobalPixel(int x, int y, bool state) {
  if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;

  int rowBlock = y / 8;
  int colBlock = x / 8;
  int moduleIndex = (rowBlock * 4) + colBlock;

  int localX = x % 8;
  int localY = y % 8;

  int finalCol = (moduleIndex * 8) + localX;
  mx.setPoint(localY, finalCol, state);
}

void afisareLabirint() {
  mx.clear();
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      if (maze[y][x] == 1) setGlobalPixel(x, y, true);
    }
  }
}

void resetare() {
  for (int y = 0; y < SCREEN_HEIGHT; y++)
    for (int x = 0; x < SCREEN_WIDTH; x++)
      maze[y][x] = 1;
}

void aplicaChenarExterior() {
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    maze[0][x] = 1;
    maze[SCREEN_HEIGHT - 1][x] = 1;
  }
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    maze[y][0] = 1;
    maze[y][SCREEN_WIDTH - 1] = 1;
  }
}

static inline void carveLine(int x0, int y0, int x1, int y1) {
  if (x0 == x1) {
    int ya = min(y0, y1), yb = max(y0, y1);
    for (int y = ya; y <= yb; y++) maze[y][x0] = 0;
  } else if (y0 == y1) {
    int xa = min(x0, x1), xb = max(x0, x1);
    for (int x = xa; x <= xb; x++) maze[y0][x] = 0;
  }
}

void generareLabirint() {
  aplicaChenarExterior();

  const int g = grosimePerete;
  const int distanta_noduri = g + 1;

  std::vector<int> xs, ys;
  xs.reserve(32);
  ys.reserve(32);

  xs.push_back(1);
  while (true) {
    int nxt = xs.back() + distanta_noduri;
    if (nxt >= 30) break;
    xs.push_back(nxt);
  }
  if (xs.back() != 30) xs.push_back(30);

  ys.push_back(1);
  while (true) {
    int nxt = ys.back() + distanta_noduri;
    if (nxt >= 30) break;
    ys.push_back(nxt);
  }
  if (ys.back() != 30) ys.push_back(30);

  const int nx = (int)xs.size();
  const int ny = (int)ys.size();

  struct Node { int ix, iy; };
  std::vector<Node> st;
  st.reserve(nx * ny);

  std::vector<std::vector<bool>> vis(ny, std::vector<bool>(nx, false));

  randomSeed(esp_random());

  vis[0][0] = true;
  maze[ys[0]][xs[0]] = 0;
  st.push_back({0, 0});

  const int dir4[4][2] = {{0,-1},{0,1},{-1,0},{1,0}}; //sus jos stanga dreapta

  while (!st.empty()) {
    Node cur = st.back();

    int cand[4];
    int cnt = 0;

    for (int i = 0; i < 4; i++) {
      int nix = cur.ix + dir4[i][0];
      int niy = cur.iy + dir4[i][1];
      if (nix >= 0 && nix < nx && niy >= 0 && niy < ny) {
        if (!vis[niy][nix]) cand[cnt++] = i;
      }
    }

    if (cnt > 0) {
      int pickDir = cand[random(cnt)];
      int nix = cur.ix + dir4[pickDir][0];
      int niy = cur.iy + dir4[pickDir][1];

      int x0 = xs[cur.ix], y0 = ys[cur.iy];
      int x1 = xs[nix],    y1 = ys[niy];

      carveLine(x0, y0, x1, y1);

      vis[niy][nix] = true;
      st.push_back({nix, niy});
    } else {
      st.pop_back();
    }
  }

  aplicaChenarExterior();
}

struct DestCand { int x; int y; bool vertical; };

void creareDestinatie() {
  std::vector<DestCand> cands;
  cands.reserve(200);

  for (int x = 1; x <= 29; x++) {
    if (maze[1][x] == 0 && maze[1][x + 1] == 0) cands.push_back({x, 0, false});
  }
  for (int x = 1; x <= 29; x++) {
    if (maze[30][x] == 0 && maze[30][x + 1] == 0) cands.push_back({x, 31, false});
  }
  for (int y = 1; y <= 29; y++) {
    if (maze[y][1] == 0 && maze[y + 1][1] == 0) cands.push_back({0, y, true});
  }
  for (int y = 1; y <= 29; y++) {
    if (maze[y][30] == 0 && maze[y + 1][30] == 0) cands.push_back({31, y, true});
  }

  if (cands.empty()) {
    destX = 1; destY = 0; destVertical = false;
    maze[0][1] = 0; maze[0][2] = 0;
    maze[1][1] = 0; maze[1][2] = 0;
  } else {
    auto c = cands[random(cands.size())];
    destX = c.x; destY = c.y; destVertical = c.vertical;
    maze[destY][destX] = 0;
    if (!destVertical) maze[destY][destX + 1] = 0;
    else              maze[destY + 1][destX] = 0;
  }
}

void placePlayer() {
  int attempts = 0;
  while (attempts < 2000) {
    int rx = random(1, SCREEN_WIDTH - 1);
    int ry = random(1, SCREEN_HEIGHT - 1);
    if (maze[ry][rx] == 0) {
      if (abs(rx - destX) > 5 || abs(ry - destY) > 5) {
        playerX = rx; playerY = ry;
        return;
      }
    }
    attempts++;
  }
  playerX = 1; playerY = 1;
  maze[playerY][playerX] = 0;
}

bool verificaDrum() {
  bool visited[32][32] = {0};
  std::vector<Point> q;
  q.reserve(32 * 32);

  if (maze[playerY][playerX] == 1) return false;

  q.push_back({playerX, playerY});
  visited[playerY][playerX] = true;

  size_t head = 0;
  while (head < q.size()) {
    Point c = q[head++];
    if (!destVertical) {
      if ((c.x == destX || c.x == destX + 1) && c.y == destY) return true;
    } else {
      if (c.x == destX && (c.y == destY || c.y == destY + 1)) return true;
    }

    const int d[4][2] = {{0,-1},{0,1},{-1,0},{1,0}};
    for (int i = 0; i < 4; i++) {
      int nx = c.x + d[i][0];
      int ny = c.y + d[i][1];
      if (nx >= 0 && nx < SCREEN_WIDTH && ny >= 0 && ny < SCREEN_HEIGHT) {
        if (maze[ny][nx] == 0 && !visited[ny][nx]) {
          visited[ny][nx] = true;
          q.push_back({nx, ny});
        }
      }
    }
  }
  return false;
}

void drawRect(int x, int y, int w, int h, bool on = true) {
  for (int yy = y; yy < y + h; yy++)
    for (int xx = x; xx < x + w; xx++)
      setGlobalPixel(xx, yy, on);
}

void drawThickLine(int x0, int y0, int x1, int y1, int thick) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy;

  while (true) {
    int r = thick / 2;
    for (int yy = y0 - r; yy <= y0 + r; yy++)
      for (int xx = x0 - r; xx <= x0 + r; xx++)
        setGlobalPixel(xx, yy, true);

    if (x0 == x1 && y0 == y1) break;
    int e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void drawVVW(int x, int y, int w, int h) {
  const int yTop = y;
  const int yBot = y + h - 1;

  int xTopL = x;
  int xTopM = x + (w / 2);
  int xTopR = x + w - 1;

  int xBotL = x + (w / 3);
  int xBotR = x + (2 * w / 3);

  drawThickLine(xTopL, yTop, xBotL, yBot, 1);
  drawThickLine(xTopL + 1, yTop, xBotL + 1, yBot, 1);

  drawThickLine(xTopM, yTop, xBotL, yBot, 1);
  drawThickLine(xTopM + 1, yTop, xBotL + 1, yBot, 1);

  drawThickLine(xTopM, yTop, xBotR, yBot, 1);
  drawThickLine(xTopM + 1, yTop, xBotR + 1, yBot, 1);

  drawThickLine(xTopR, yTop, xBotR, yBot, 1);
  drawThickLine(xTopR + 1, yTop, xBotR + 1, yBot, 1);

  drawRect(xBotL, yBot - 1, 2, 2, true);
  drawRect(xBotR, yBot - 1, 2, 2, true);
}

void drawBoldI(int x, int y, int w, int h) {
  const int bottomY = y + h - 1;
  drawRect(x, y, w, 2, true);
  drawRect(x, bottomY - 1, w, 2, true);
  drawRect(x + 1, y, w - 2, h, true);
}

void drawBoldN(int x, int y, int w, int h) {
  const int stroke = 2;
  const int bottomY = y + h - 1;

  drawRect(x, y, stroke, h, true);
  drawRect(x + w - stroke, y, stroke, h, true);

  drawThickLine(x + stroke, y, x + w - stroke - 1, bottomY, 1);
  drawThickLine(x + stroke + 1, y, x + w - stroke, bottomY, 1);
}

void drawWinScreen() {
  mx.clear();
  const int top = 5;
  const int h = 22;

  const int xW = 1;
  const int xI = xW + 12 + 1;
  const int xN = xI + 4 + 1;

  drawVVW(xW, top, 12, h);
  drawBoldI(xI, top, 4, h);
  drawBoldN(xN, top, 12, h);
}

void checkWin() {
  bool win = false;

  if (!destVertical) {
    if ((playerX == destX || playerX == destX + 1) && playerY == destY) win = true;
  } else {
    if (playerX == destX && (playerY == destY || playerY == destY + 1)) win = true;
  }

  if (win) {
    Serial.println("WIN!");
    gameOver = true;
    drawWinScreen();
  }
}

void movePlayer(int dx, int dy) {
  int newX = playerX + dx;
  int newY = playerY + dy;

  if (newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT) {
    if (maze[newY][newX] == 0) {
      setGlobalPixel(playerX, playerY, false);
      playerX = newX; playerY = newY;
      setGlobalPixel(playerX, playerY, true);
      checkWin();
    }
  }
}

void handleInput() {
  if (IrReceiver.decode()) {
    uint8_t cmd = IrReceiver.decodedIRData.command;

    if (cmd == 0x18) movePlayer(0, -1);
    if (cmd == 0x4A) movePlayer(0,  1);
    if (cmd == 0x10) movePlayer(-1, 0);
    if (cmd == 0x5A) movePlayer( 1, 0);

    IrReceiver.resume();
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("BOOT OK");

  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 2);
  mx.clear();

  IrReceiver.begin(IR_PIN, DISABLE_LED_FEEDBACK);

  int tries = 0;
  do {
    resetare();
    generareLabirint();
    creareDestinatie();
    placePlayer();
    tries++;
  } while (!verificaDrum() && tries < 80);

  afisareLabirint();
  setGlobalPixel(playerX, playerY, true);

  Serial.println("Game started!");
}

void loop() {
  if (gameOver) {
    delay(1000);
    return;
  }

  handleInput();

  if (gameOver) {
    delay(1000);
    return;
  }

  static unsigned long lastBlink = 0;
  static bool ledState = false;
  unsigned long now = millis();

  if (now - lastBlink > 100) {
    lastBlink = now;
    ledState = !ledState;

    setGlobalPixel(destX, destY, ledState);
    if (!destVertical) setGlobalPixel(destX + 1, destY, ledState);
    else              setGlobalPixel(destX, destY + 1, ledState);

    static int pCnt = 0; pCnt++;
    if (pCnt > 3) {
      setGlobalPixel(playerX, playerY, true);
      if (pCnt > 6) pCnt = 0;
    } else {
      setGlobalPixel(playerX, playerY, false);
    }
  }
}
