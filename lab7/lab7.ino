#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 3, 2, 1, 0);

byte gol[8] =      {B11111,B10001,B10001,B10001,B10001,B10001,B10001,B11111};
byte sfert[8] =    {B11111,B10001,B10001,B10001,B10001,B11111,B11111,B11111};
byte jumatate[8] = {B11111,B10001,B10001,B11111,B11111,B11111,B11111,B11111};
byte plin[8] =     {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};

int nivel = 0;
int modCurent = 1;
unsigned long vitezaAnimatie = 500; 

unsigned long ultimulNivel = 0;
unsigned long ultimulPuls = 0;
int framePuls = 0;

int readKey() {
  int x = analogRead(A0);
  if (x < 50) return 0;
  if (x < 200) return 1;
  if (x < 400) return 2;
  if (x < 600) return 3;
  if (x < 800) return 4;
  return -1;
}

void afiseazaBaterie(int pct) {
  int celule = 20; 
  int umplute = map(pct, 0, 100, 0, celule);
  
  lcd.setCursor(0, 2);
  for (int i = 0; i < celule; i++) {
    if (i < umplute) {
      if (pct < 100) {
        lcd.write((uint8_t)(framePuls == 0 ? 2 : 3));
      } else {
        lcd.write((uint8_t)3);
      }
    } else {
      lcd.write((uint8_t)0);
    }
  }
}

void afiseazaStatus() {
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  if (modCurent == 1) {
    lcd.print("MOD: NORMAL");
    vitezaAnimatie = 500;
  } else if (modCurent == 2) {
    lcd.print("MOD: RAPID (x3)");
    vitezaAnimatie = 166;
  } else {
    lcd.print("MOD: STANDBY");
  }
}

void setup() {
  lcd.begin(20, 4);
  lcd.clear();

  lcd.createChar(0, gol);
  lcd.createChar(1, sfert);
  lcd.createChar(2, jumatate);
  lcd.createChar(3, plin);

  lcd.setCursor(0, 0);
  lcd.print("SIMULARE INCARCARE");
  afiseazaStatus();
}

void handleKeys() {
  int k = readKey();
  unsigned long t = millis();
  
  if (k == 1) {
    if (t - ultimulPuls > 200) modCurent = 1;
    afiseazaStatus();
  }
  if (k == 2) {
    if (t - ultimulPuls > 200) modCurent = 2;
    afiseazaStatus();
  }
  if (k == 4) {
    if (t - ultimulPuls > 200) modCurent = 3;
    afiseazaStatus();
  }
}

void loop() {
  handleKeys();
  unsigned long t = millis();

  if (t - ultimulPuls >= 150) {
    ultimulPuls = t;
    framePuls = 1 - framePuls; 
    afiseazaBaterie(nivel);
  }

  if (modCurent != 3 && nivel < 100) {
    if (t - ultimulNivel >= vitezaAnimatie) {
      ultimulNivel = t;
      nivel++;
      lcd.setCursor(0, 3);
      lcd.print("Nivel: ");
      lcd.print(nivel);
      lcd.print("%          ");
      afiseazaBaterie(nivel);
    }
  }
}