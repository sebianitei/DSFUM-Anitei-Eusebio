#define PIN_START 2
#define NUMAR_BITI 4

void setup() {
  for (int p = PIN_START; p < PIN_START + NUMAR_BITI; p++) {
    pinMode(p, OUTPUT);
  }
}

void codificaStare(int val) {
  for (int bit = 0; bit < NUMAR_BITI; bit++) {
    int stare = (val >> bit) & 1;
    digitalWrite(PIN_START + bit, stare);
  }
}

void loop() {
  int stare_curenta = (millis() % 8000) / 500;
  codificaStare(stare_curenta);
}