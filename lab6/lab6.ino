#include <LedControl.h>

LedControl ecran(9, 7, 6, 1);

byte camp_microbi[10];
byte urmatorul_camp[10];
byte stare_veche[10];
int pasi_stabili = 0;

bool citesteCelule(byte g[10], int r, int c) {
  if (r < 1 || r > 8 || c < 0 || c > 7) return 0;
  return (g[r] >> c) & 1;
}

byte numarVecini(byte g[10], int r, int c) {
  byte n = 0;
  for (int dr = -1; dr <= 1; dr++)
    for (int dc = -1; dc <= 1; dc++)
      if (!(dr == 0 && dc == 0))
        n += citesteCelule(g, r + dr, c + dc);
  return n;
}

void progres() {
  for (int r = 1; r <= 8; r++) {
    urmatorul_camp[r] = 0;
    for (int c = 0; c < 8; c++) {
      byte activ = citesteCelule(camp_microbi, r, c);
      byte n = numarVecini(camp_microbi, r, c);
      if ((activ && (n >= 3 && n <= 5)) || (!activ && n == 4))
        urmatorul_camp[r] |= (1 << c);
    }
  }
}

void vizualizeaza(byte g[10]) {
  for (int r = 1; r <= 8; r++)
    ecran.setRow(0, r - 1, g[r]);
}

bool laFel(byte a[10], byte b[10]) {
  for (int i = 1; i <= 8; i++)
    if (a[i] != b[i]) return false;
  return true;
}

void initializareCamp() {
  for (int i = 1; i <= 8; i++)
    camp_microbi[i] = random(0, 256);
}

void setup() {
  ecran.shutdown(0, false);
  ecran.setIntensity(0, 8);
  ecran.clearDisplay(0);

  randomSeed(analogRead(A0));
  initializareCamp();
  vizualizeaza(camp_microbi);
}

void loop() {
  delay(150);

  for (int i = 1; i <= 8; i++) stare_veche[i] = camp_microbi[i];

  progres();
  vizualizeaza(urmatorul_camp);

  bool stabilitate = laFel(camp_microbi, urmatorul_camp);
  for (int i = 1; i <= 8; i++) camp_microbi[i] = urmatorul_camp[i];

  if (stabilitate) {
    pasi_stabili++;
  } else {
    pasi_stabili = 0;
  }

  if (pasi_stabili >= 20) {
    delay(500);
    initializareCamp();
    vizualizeaza(camp_microbi);
    pasi_stabili = 0;
  }
}