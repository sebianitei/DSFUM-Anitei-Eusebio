#define PIN_START 1
#define PIN_FINAL 5

int pozitie = PIN_START;
int directie = 1; 
void setup() {
  for(int p = PIN_START; p <= PIN_FINAL; p++) {
    pinMode(p, OUTPUT);
  }
}

void loop() {
  digitalWrite(pozitie, HIGH);
  delay(100);

  digitalWrite(pozitie, LOW);
  delay(100);

  pozitie = pozitie + directie;

  if (pozitie > PIN_FINAL) {
    pozitie = PIN_FINAL - 1; 
    directie = -1;
  } 
  
  if (pozitie < PIN_START) {
    pozitie = PIN_START + 1; 
    directie = 1;
  }
}