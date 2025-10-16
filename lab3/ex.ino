struct EfectLuminos {
  bool estePornit = false;
  String stil = "fix";
  unsigned long rata_ciclu = 0;
  unsigned long timp_ultima_schimbare = 0;
  int nivel_pwm = 0;
  int sens_variatie = 1; 
};

EfectLuminos lumini[10];
String bufferSerial;

void setup() {
  Serial.begin(9600);
  for (int p = 2; p <= 9; p++) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }
  Serial.println("Comanda: pin:stil:rata (ex: 3:puls:100)");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      citesteComanda(bufferSerial);
      bufferSerial = "";
    } else if (c != '\r') {
      bufferSerial += c;
    }
  }

  unsigned long acum = millis();
  for (int p = 2; p <= 9; p++) {
    if (lumini[p].estePornit) {
      aplicaEfect(p, acum);
    }
  }
}

void citesteComanda(String s) {
  s.trim();
  int c1 = s.indexOf(':');
  int c2 = s.indexOf(':', c1 + 1);

  if (c1 < 0 || c2 < 0) {
    Serial.println("Format invalid");
    return;
  }

  int pin = s.substring(0, c1).toInt();
  String stil = s.substring(c1 + 1, c2);
  unsigned long rata = s.substring(c2 + 1).toInt();

  if (pin < 2 || pin > 9) {
    Serial.println("Pin invalid");
    return;
  }

  lumini[pin].estePornit = true;
  lumini[pin].stil = stil;
  lumini[pin].rata_ciclu = rata;
  lumini[pin].timp_ultima_schimbare = millis();

  if (stil == "ondulat") {
    lumini[pin].nivel_pwm = 0;
    lumini[pin].sens_variatie = 1;
  }
  
  if (stil == "fix") digitalWrite(pin, HIGH); 
  Serial.print("OK pin ");
  Serial.print(pin);
  Serial.print(" stil ");
  Serial.println(stil);
}

void aplicaEfect(int pin, unsigned long acum) {
  EfectLuminos &L = lumini[pin];
  
  if (L.stil == "fix") {
    return; 
  }

  if (L.stil == "puls") {
    if (acum - L.timp_ultima_schimbare >= L.rata_ciclu) {
      L.timp_ultima_schimbare = acum;
      digitalWrite(pin, !digitalRead(pin));
    }
    return;
  }

  if (L.stil == "ondulat") {
    unsigned long pas_schimbare = L.rata_ciclu / 255;
    if (pas_schimbare < 1) pas_schimbare = 1;

    if (acum - L.timp_ultima_schimbare >= pas_schimbare) {
      L.timp_ultima_schimbare = acum;

      L.nivel_pwm += L.sens_variatie;
      
      if (L.nivel_pwm >= 255 || L.nivel_pwm <= 0) {
        L.sens_variatie = -L.sens_variatie;
      }
    }

    int perioada_pwm = 8;
    unsigned long faza = acum % perioada_pwm;
    int timp_on = (L.nivel_pwm * perioada_pwm) / 255;
    digitalWrite(pin, faza < timp_on);
  }
}