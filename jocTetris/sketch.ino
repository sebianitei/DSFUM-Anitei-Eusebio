#include <LedControl.h>

#define LED_DIN 11
#define LED_CS 10
#define LED_CLK 13
#define NUM_DEVICES 8

#define SR_DATA 9
#define SR_LATCH 8
#define SR_CLOCK 7

#define BUTON_SUS 41
#define BUTON_STANGA 43
#define BUTON_DREAPTA 45
#define BUTON_JOS 47
#define BUZZER_PIN 49

#define LATIME_TEREN 16
#define INALTIME_TEREN 32

byte terenJoc[INALTIME_TEREN][LATIME_TEREN];

LedControl matrice = LedControl(LED_DIN, LED_CLK, LED_CS, NUM_DEVICES);

struct Pozitie {
  int col;
  int rand;
};

Pozitie pozCurenta;
int tipPiesa;
int orientare;

long punctaj = 0;
int vitezaCadere = 500;
bool jocTerminat = false;

byte cifre[4] = {0, 0, 0, 0};

const byte codCifre[] = {
  0xAF, 0x28, 0xCD, 0x6D, 0x2A, 0x67, 0xE7, 0x0C, 0xEF, 0x6F
};

const byte selectDigit[] = {
  0x20, 0x40, 0x80, 0x01
};

unsigned long momentCadere = 0;
unsigned long momentInput = 0;
unsigned long momentAfisare = 0;

const int formePiese[7][4][4][2] = {
  {{{0,1},{1,1},{2,1},{3,1}}, {{2,0},{2,1},{2,2},{2,3}}, {{0,2},{1,2},{2,2},{3,2}}, {{1,0},{1,1},{1,2},{1,3}}}, // I - bara
  {{{0,0},{0,1},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{1,2}}, {{0,1},{1,1},{2,1},{2,2}}, {{1,0},{1,1},{0,2},{1,2}}}, // J - colt stanga
  {{{2,0},{0,1},{1,1},{2,1}}, {{1,0},{1,1},{1,2},{2,2}}, {{0,1},{1,1},{2,1},{0,2}}, {{0,0},{1,0},{1,1},{1,2}}}, // L - colt dreapta
  {{{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}, {{1,0},{2,0},{1,1},{2,1}}}, // O - patrat
  {{{1,0},{2,0},{0,1},{1,1}}, {{1,0},{1,1},{2,1},{2,2}}, {{1,1},{2,1},{0,2},{1,2}}, {{0,0},{0,1},{1,1},{1,2}}}, // S - zigzag
  {{{1,0},{0,1},{1,1},{2,1}}, {{1,0},{1,1},{2,1},{1,2}}, {{0,1},{1,1},{2,1},{1,2}}, {{1,0},{0,1},{1,1},{1,2}}}, // T - forma T
  {{{0,0},{1,0},{1,1},{2,1}}, {{2,0},{1,1},{2,1},{1,2}}, {{0,1},{1,1},{1,2},{2,2}}, {{1,0},{0,1},{1,1},{0,2}}}  // Z - zigzag invers
};

void emiteSunet(int tip) {
  switch(tip) {
    case 0: tone(BUZZER_PIN, 600, 10); break; 
    case 1: tone(BUZZER_PIN, 700, 10); break; 
    case 2: tone(BUZZER_PIN, 200, 40); break; 
    case 3: 
      tone(BUZZER_PIN, 500, 50); 
      delay(50);
      tone(BUZZER_PIN, 750, 50); 
      delay(50);
      tone(BUZZER_PIN, 1000, 100); 
      break;
    case 4: 
      tone(BUZZER_PIN, 400, 150); 
      delay(150);
      tone(BUZZER_PIN, 300, 300);
      break;
  }
}

void actualizareCifre() {
  long temp = punctaj;
  cifre[0] = (temp / 1000) % 10;
  cifre[1] = (temp / 100) % 10;
  cifre[2] = (temp / 10) % 10;
  cifre[3] = temp % 10;
}

void afiseazaScor() {
  static int indexDigit = 0;

  digitalWrite(SR_LATCH, LOW);
  shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, selectDigit[indexDigit]);
  shiftOut(SR_DATA, SR_CLOCK, MSBFIRST, ~codCifre[cifre[indexDigit]]);
  digitalWrite(SR_LATCH, HIGH);
  
  indexDigit = (indexDigit + 1) % 4;
}

void setPixel(int col, int rand, bool stare) {
  if (col < 0 || col >= LATIME_TEREN || rand < 0 || rand >= INALTIME_TEREN) 
    return;

  int dispozitiv, linie, coloana;
  int sectiune = rand / 8;

  if (col >= 8) {
    dispozitiv = sectiune;
    linie = col - 8;
    coloana = rand % 8;
  } else {
    dispozitiv = 7 - sectiune;
    linie = 7 - col;
    coloana = 7 - (rand % 8);
  }

  matrice.setLed(dispozitiv, linie, coloana, stare);
}

void redeseneazaEcran() {
  for(int i = 0; i < NUM_DEVICES; i++) 
    matrice.clearDisplay(i);
  
  for(int r = 0; r < INALTIME_TEREN; r++) {
    for(int c = 0; c < LATIME_TEREN; c++) {
      if(terenJoc[r][c]) 
        setPixel(c, r, true);
    }
  }
  
  if(!jocTerminat) {
    for(int i = 0; i < 4; i++) {
      int offsetX = formePiese[tipPiesa][orientare][i][0];
      int offsetY = formePiese[tipPiesa][orientare][i][1];
      setPixel(pozCurenta.col + offsetX, pozCurenta.rand + offsetY, true);
    }
  }
}

bool verificaColiziune(int testCol, int testRand, int testOrientare) {
  for(int i = 0; i < 4; i++) {
    int x = testCol + formePiese[tipPiesa][testOrientare][i][0];
    int y = testRand + formePiese[tipPiesa][testOrientare][i][1];

    if(x < 0 || x >= LATIME_TEREN || y >= INALTIME_TEREN) 
      return true;
    
    if(y >= 0 && terenJoc[y][x]) 
      return true;
  }
  return false;
}


void verificaLiniiComplete() {
  int liniiEliminateAcum = 0;

  for(int r = INALTIME_TEREN - 1; r >= 0; r--) {
    bool linieCompleta = true;
    
    for(int c = 0; c < LATIME_TEREN; c++) {
      if(terenJoc[r][c] == 0) {
        linieCompleta = false;
        break;
      }
    }

    if(linieCompleta) {
      liniiEliminateAcum++;
      
      
      for(int k = r; k > 0; k--) {
        for(int c = 0; c < LATIME_TEREN; c++) {
          terenJoc[k][c] = terenJoc[k-1][c];
        }
      }
      
      
      for(int c = 0; c < LATIME_TEREN; c++) 
        terenJoc[0][c] = 0;
      
      r++; 
    }
  }

  if(liniiEliminateAcum > 0) {
    punctaj += liniiEliminateAcum;
    actualizareCifre();
    
    vitezaCadere = max(50, 500 - (int)(punctaj * 10));
    
    emiteSunet(3);
  }
}

void genereazaPiesaNoua() {
  tipPiesa = random(0, 7);
  orientare = 0;
  pozCurenta.col = 6;
  pozCurenta.rand = -1;

  if(verificaColiziune(pozCurenta.col, pozCurenta.rand, orientare)) {
    jocTerminat = true;
    emiteSunet(4);
  }
}

void fixeazaPiesa() {
  for(int i = 0; i < 4; i++) {
    int x = pozCurenta.col + formePiese[tipPiesa][orientare][i][0];
    int y = pozCurenta.rand + formePiese[tipPiesa][orientare][i][1];
    
    if(y >= 0) 
      terenJoc[y][x] = 1;
  }
  
  emiteSunet(2);
  verificaLiniiComplete();
  genereazaPiesaNoua();
}

void restartJoc() {
  memset(terenJoc, 0, sizeof(terenJoc));
  punctaj = 0;
  vitezaCadere = 500;
  jocTerminat = false;
  actualizareCifre();
  genereazaPiesaNoua();
}

void setup() {
  for(int i = 0; i < NUM_DEVICES; i++) {
    matrice.shutdown(i, false);
    matrice.setIntensity(i, 4);
    matrice.clearDisplay(i);
  }

  pinMode(BUTON_SUS, INPUT);
  pinMode(BUTON_STANGA, INPUT);
  pinMode(BUTON_DREAPTA, INPUT);
  pinMode(BUTON_JOS, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SR_DATA, OUTPUT);
  pinMode(SR_LATCH, OUTPUT);
  pinMode(SR_CLOCK, OUTPUT);

  randomSeed(analogRead(A0));
  restartJoc();
}

void loop() {
  unsigned long acum = millis();

  if(acum - momentAfisare > 3) {
    afiseazaScor();
    momentAfisare = acum;
  }

  if(jocTerminat) {
    if(digitalRead(BUTON_SUS)) {
      restartJoc();
      delay(500);
    }
    return;
  }

  if(acum - momentInput > 100) {
    if(digitalRead(BUTON_STANGA) && !verificaColiziune(pozCurenta.col - 1, pozCurenta.rand, orientare)) {
      pozCurenta.col--;
      emiteSunet(0);
      redeseneazaEcran();
    }
    
    if(digitalRead(BUTON_DREAPTA) && !verificaColiziune(pozCurenta.col + 1, pozCurenta.rand, orientare)) {
      pozCurenta.col++;
      emiteSunet(0);
      redeseneazaEcran();
    }
    
    if(digitalRead(BUTON_SUS)) {
      int orientareNoua = (orientare + 1) % 4;
      if(!verificaColiziune(pozCurenta.col, pozCurenta.rand, orientareNoua)) {
        orientare = orientareNoua;
        emiteSunet(1);
        redeseneazaEcran();
      }
    }
    
    momentInput = acum;
  }

  int intervalCadere = digitalRead(BUTON_JOS) ? 50 : vitezaCadere;
  
  if(acum - momentCadere > intervalCadere) {
    if(!verificaColiziune(pozCurenta.col, pozCurenta.rand + 1, orientare)) {
      pozCurenta.rand++;
    } else {
      fixeazaPiesa();
    }
    
    redeseneazaEcran();
    momentCadere = acum;
  }
}