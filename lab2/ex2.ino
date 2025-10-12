unsigned long lvl6 = 0;
int step6 = 2;
unsigned long prev6 = 0;
int speed6 = 40;

unsigned long lvl7 = 0;
int step7 = 8;
unsigned long prev7 = 0;
int speed7 = 15;

int perioada = 10;

void setup() {
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop() {
  unsigned long timp_acum = millis();

  if (timp_acum - prev6 >= speed6) {
    prev6 = timp_acum;
    lvl6 += step6;
    if (lvl6 >= 255 || lvl6 == 0) {
      step6 = -step6;
    }
  }

  int ciclu6 = timp_acum % perioada;
  int duty6 = (lvl6 * perioada) / 255;
  digitalWrite(6, ciclu6 < duty6);

  if (timp_acum - prev7 >= speed7) {
    prev7 = timp_acum;
    lvl7 += step7;
    if (lvl7 >= 255 || lvl7 == 0) {
      step7 = -step7;
    }
  }

  int ciclu7 = timp_acum % perioada;
  int duty7 = (lvl7 * perioada) / 255;
  digitalWrite(7, ciclu7 < duty7);
}