/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

#define DOT 50
#define DASH 200
#define SLEEP 100
// how long between letters
#define SPACE 300
// pin for LED
#define LED 13

void sig(int ms) {
  digitalWrite(LED, HIGH);
  delay(ms);
  digitalWrite(LED, LOW);
  delay(SLEEP);
}

void dot() { sig(DOT); }
void dash() { sig(DASH); }

void space() {
  digitalWrite(LED, LOW);    // set the LED off
  delay(SPACE);
}

void s() {
  for (int i = 0; i < 3; i++) {
    dot();
  }
  space();
}

void o() {
  for (int i = 0; i < 3; i++) {
    dash();
  }
  space();
}

void setup() {
  pinMode(LED, OUTPUT);
  space(); space();
  Serial.begin(9600);
}

void loop() {
  Serial.println("hello");
  s(); o(); s();
  space();
}
