#define PIN_PRINCIPAL 10
#define PIN_SECUNDAR 12

void setup() {
  for (int p = 10; p <= 15; p++) {
    pinMode(p, OUTPUT);
  }
}

void luminaPrincipala() {
  int stare = (millis() / 2000) % 2;
  digitalWrite(PIN_PRINCIPAL, stare);
}

void luminaSecundara() {
  int stare = (millis() / 6000) % 2;
  digitalWrite(PIN_SECUNDAR, stare);
}

void loop() {
  luminaPrincipala();
  luminaSecundara();
}