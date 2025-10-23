#define PIN_LATCH 10
#define PIN_CLOCK 11
#define PIN_DATA 12

int secventaCorecta[5];
unsigned long timpReactie[5];

void setup() {
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);

  Serial.begin(9600);
  randomSeed(analogRead(A0));

  Serial.println("Introdu rapid secventa");
  delay(1500);
}

void trimiteMasca(byte masca) {
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, LSBFIRST, masca);
  digitalWrite(PIN_LATCH, HIGH);
}

void afiseazaLED(int led_index) {
  byte masca = 1 << (led_index - 1);
  trimiteMasca(masca);
  delay(300);
  trimiteMasca(0);
  delay(100);
}

void emiteSecventa() {
  trimiteMasca(0);

  for (int i = 0; i < 5; i++) {
    secventaCorecta[i] = random(1, 5);
    afiseazaLED(secventaCorecta[i]);
  }
}

bool citesteSiVerifica() {
  String s = Serial.readStringUntil('\n');
  s.trim();
  int raspunsUtilizator[5];
  int count = 0;

  for (int i = 0; i < s.length() && count < 5; i++) {
    char c = s.charAt(i);
    if (isDigit(c)) {
      raspunsUtilizator[count] = c - '0';
      count++;
    }
  }

  if (count != 5) return false;

  for (int i = 0; i < 5; i++) {
    if (raspunsUtilizator[i] != secventaCorecta[i]) {
      return false;
    }
  }
  return true;
}

void loop() {

  for (int incercare = 0; incercare < 5; incercare++) {
    Serial.println("\nSecventa noua");
    delay(1000);
    
    emiteSecventa(); 
    Serial.println("Introdu secventa de 5 numere:");

    unsigned long start = millis();
    bool gata = false;

    while (!gata) {
      if (Serial.available()) {
        if (citesteSiVerifica()) {
          gata = true;
          timpReactie[incercare] = millis() - start;
          Serial.print("Corect, timp: ");
          Serial.print(timpReactie[incercare]);
          Serial.println(" ms");
        } else {
          Serial.println("Gresit. Introdu din nou secventa.");
        }
      }
    }

    delay(500);
  }

  unsigned long total = 0;
  unsigned long minT = timpReactie[0], maxT = timpReactie[0];

  for (int i = 0; i < 5; i++) {
    if (timpReactie[i] < minT) minT = timpReactie[i];
    if (timpReactie[i] > maxT) maxT = timpReactie[i];
    total += timpReactie[i];
  }

  float medie = total / 5.0;

  Serial.println("\nRezultat final");
  Serial.print("Minim: "); Serial.print(minT); Serial.println(" ms");
  Serial.print("Maxim: "); Serial.print(maxT); Serial.println(" ms");
  Serial.print("Medie: "); Serial.print(medie); Serial.println(" ms");

  while (1);
}