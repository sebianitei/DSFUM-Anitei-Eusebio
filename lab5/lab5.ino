#define SONOR_PIN 6

int tonuri[] = {
  440, 494, 554, 587, 659, 740, 831, 880
};

const int numarTonuri = sizeof(tonuri) / sizeof(tonuri[0]);
const int durataPuls = 150;

void setup() {
  pinMode(SONOR_PIN, OUTPUT);
}

void emiteTon(int f) {
  tone(SONOR_PIN, f, durataPuls);
  delay(durataPuls + 20);
  noTone(SONOR_PIN);
}

void loop() {

  for(int i = 0; i < numarTonuri; i++) {
    emiteTon(tonuri[i]);
  }

  int secventaRapida[] = {0, 1, 2, 3, 2, 1, 0};
  
  for(int i = 0; i < sizeof(secventaRapida) / sizeof(secventaRapida[0]); i++) {
    emiteTon(tonuri[secventaRapida[i]]);
  }

  delay(1200);
}