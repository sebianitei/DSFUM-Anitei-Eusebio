#define PIN_LATCH 10
#define PIN_CLOCK 11
#define PIN_DATA 12

void setup() {
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
}

void expediazaDate(byte seg, byte digitMask) {
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, LSBFIRST, seg);
  shiftOut(PIN_DATA, PIN_CLOCK, LSBFIRST, digitMask);
  digitalWrite(PIN_LATCH, HIGH);
}

void loop() {
  byte coduriCifre[10] = {
    B01000001, 
    B00000001, 
    B11100011,
    B00001001,
    B01001001,
    B11010001,
    B01100001,
    B00100101,
    B11110011,
    B00000011
  };

  byte selectieDigit[3] = { B11111110, B11111101, B11111011 }; 

  for (int digit = 0; digit < 3; digit++) {

    for (int index = 0; index < 10; index++) {

      expediazaDate(coduriCifre[index], selectieDigit[digit]);
      delay(500); 
    }
  }
}