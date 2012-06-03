/*
  A program to demonstrate turning circuits on and off programmatically.
 */

// pin for LEDs
#define LED00 12
#define LED01 11
#define LED02 10
#define LED03 9

// how many circuits are there?
#define COUNT 4

// how many to light at one time
#define LIT 1

// lower is faster
#define SPEED0 100
#define SPEED1 300

// how many times to run each demo part
#define DEMOLENGTH 40

// array of leds
int LEDS[] = {LED00, LED01, LED02, LED03};

void setup() {
  // set all pins for output
  for (int i = 0; i < COUNT; ++i) {
    pinMode(LEDS[i], OUTPUT);
  }
  Serial.begin(9600);
}

int timer = 0;
int direction = 1;
int speed = SPEED0;

void type_loop() {
  for (int i = 0; i < COUNT; ++i) {
    // turn on two lights according to timer
    digitalWrite(LEDS[i], (timer + i) % COUNT < LIT ? HIGH : LOW);
  }
  if (timer == 4) {
    direction = -1;
    speed = SPEED0;
  }
  if (timer == 1) {
    direction = 1;
    speed = SPEED1;
  }
  delay(speed);
  timer = timer + direction;
}

void ring_loop(int direction, int speed) {
  for (int i = 0; i < COUNT; ++i) {
    // turn on two lights according to timer
    digitalWrite(LEDS[i], (timer + i) % COUNT < LIT ? HIGH : LOW);
  }
  delay(speed);
  timer = (timer + direction) % COUNT;
}

void loop() {
  for (int i = 0; i < DEMOLENGTH; ++i) {
    type_loop();
  }
  for (int i = 0; i < DEMOLENGTH; ++i) {
    ring_loop(1, SPEED0);
  }
  for (int i = 0; i < DEMOLENGTH; ++i) {
    ring_loop(-1, SPEED1);
  }
  for (int i = 0; i < DEMOLENGTH; ++i) {
    ring_loop(1, SPEED1);
  }
  for (int i = 0; i < DEMOLENGTH; ++i) {
    ring_loop(-1, SPEED0);
  }
}
