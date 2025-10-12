#define PIN_START 0
#define PIN_FINAL 7
#define NUMAR_PINI 8
#define PASI_CICLU 15 

void setup() {
  for (int p = PIN_START; p < PIN_START + NUMAR_PINI; p++) {
    pinMode(p, OUTPUT);
  }
}

void loop() {
  unsigned long timp_curent = millis();
  int index_ciclu = (timp_curent / 150) % PASI_CICLU;
  int pinActiv;

  if (index_ciclu < NUMAR_PINI) {
    pinActiv = index_ciclu;
  } else {
    pinActiv = PIN_FINAL - (index_ciclu - (NUMAR_PINI - 1));
  }

  int pinAprins = PIN_START + pinActiv;
  digitalWrite(pinAprins, HIGH);

  for (int p = PIN_START; p < PIN_START + NUMAR_PINI; p++) {
    if (p != pinAprins) digitalWrite(p, LOW);
  }
}