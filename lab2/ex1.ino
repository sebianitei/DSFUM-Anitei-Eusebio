unsigned long intensitate = 0;
int directie = 10;
unsigned long timpUltimulPas = 0;
int intervalActualizare = 50;
int perioadaPWM = 8;

void setup() {
  pinMode(9, OUTPUT);
}

void loop() {
  unsigned long timpAcum = millis();

  if (timpAcum - timpUltimulPas >= intervalActualizare) {
    timpUltimulPas = timpAcum;

    intensitate += directie;

    if (intensitate >= 255 || intensitate == 0) {
      directie = -directie;
    }
  }

  unsigned long timpInCiclu = timpAcum % perioadaPWM;

  unsigned long durataImpuls = (intensitate * perioadaPWM) / 255;

  if (timpInCiclu < durataImpuls)
    digitalWrite(9, HIGH);
  else
    digitalWrite(9, LOW);
}