#define LED_PIN 7

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int timp_start = millis();
  
  while( millis() - timp_start < 40)
  {
    if( millis() - timp_start < 8)
      digitalWrite(LED_PIN, 1);
    else
      digitalWrite(LED_PIN, 0);
  }
}