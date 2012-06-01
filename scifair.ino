/*
 * demo16x24.c - Arduino demo program for Holtek HT1632 LED driver chip,
 *      As implemented on the Sure Electronics DE-DP016 display board
 *      (16*24 dot matrix LED module.)
 * Nov, 2008 by Bill Westfield ("WestfW")
 */

#include "ht1632.h"
#define X_MAX 23
#define Y_MAX 15

#define ASSERT(condition) //nothing


/*
 * Set these constants to the values of the pins connected to the SureElectronics Module
 */
static const byte ht1632_data = 12;  // Data pin (pin 7)
static const byte ht1632_wrclk = 11; // Write clock pin (pin 5)
static const byte ht1632_cs = 10;    // Chip Select (1, 2, 3, or 4)
// The should also be a common GND.
// The module with all LEDs like draws about 200mA,
//  which makes it PROBABLY powerable via Arduino +5V

#define DEMOTIME 30000  // 30 seconds max on each demo is enough.
#define DISPDELAY 40    // Each "display" lasts this long
#define LONGDELAY 1000  // This delay BETWEEN demos


/*
 * ht1632_writebits
 * Write bits (up to 8) to h1632 on pins ht1632_data, ht1632_wrclk
 * Chip is assumed to already be chip-selected
 * Bits are shifted out from MSB to LSB, with the first bit sent
 * being (bits & firstbit), shifted till firsbit is zero.
 */
void ht1632_chipselect(byte chipno)
{
  //Serial.println("HT1632 chipselect");
  //Serial.println(chipno);
  DEBUGPRINT("\nHT1632(%d) ", chipno);
  digitalWrite(chipno, 0);
}

void ht1632_chipfree(byte chipno)
{
  //Serial.println("HT1632 chipfree");
  //Serial.println(chipno);
  DEBUGPRINT(" [done %d]", chipno);
  digitalWrite(chipno, 1);
}

void ht1632_writebits (byte bits, byte firstbit)
{
  //Serial.println("HT1632 writebits");
  //Serial.println(bits);
  //Serial.println(firstbit);
  DEBUGPRINT(" ");
  while (firstbit) {
    DEBUGPRINT((bits&firstbit ? "1" : "0"));
    digitalWrite(ht1632_wrclk, LOW);
    if (bits & firstbit) {
    digitalWrite(ht1632_data, HIGH);
    }
    else {
    digitalWrite(ht1632_data, LOW);
    }
    digitalWrite(ht1632_wrclk, HIGH);
    firstbit >>= 1;
  }
}

static void ht1632_sendcmd (byte command)
{
  //Serial.println("HT1632 sendcmd");
  //Serial.println(command);
  ht1632_chipselect(ht1632_cs);  // Select chip
  ht1632_writebits(HT1632_ID_CMD, 1<<2);  // send 3 bits of id: COMMMAND
  ht1632_writebits(command, 1<<7);  // send the actual command
  ht1632_writebits(0, 1);   /* one extra dont-care bit in commands. */
  ht1632_chipfree(ht1632_cs); //done
}

static void ht1632_senddata (byte address, byte data)
{
  //Serial.println("HT1632 senddata");
  //Serial.println(address);
  //Serial.println(data);
  ht1632_chipselect(ht1632_cs);  // Select chip
  ht1632_writebits(HT1632_ID_WR, 1<<2);  // send ID: WRITE to RAM
  ht1632_writebits(address, 1<<6); // Send address
  ht1632_writebits(data, 1<<3); // send 4 bits of data
  ht1632_chipfree(ht1632_cs); // done
}

void setup ()  // flow chart from page 17 of datasheet
{
  Serial.begin(9600);
  pinMode(ht1632_cs, OUTPUT);
  digitalWrite(ht1632_cs, HIGH);    /* unselect (active low) */
  pinMode(ht1632_wrclk, OUTPUT);
  pinMode(ht1632_data, OUTPUT);
  ht1632_sendcmd(HT1632_CMD_SYSDIS);  // Disable system
  ht1632_sendcmd(HT1632_CMD_COMS01);  // 16*32, PMOS drivers
  ht1632_sendcmd(HT1632_CMD_MSTMD);     /* Master Mode */
  ht1632_sendcmd(HT1632_CMD_SYSON);     /* System on */
  ht1632_sendcmd(HT1632_CMD_LEDON);     /* LEDs on */
  for (byte i=0; i<128; i++)
    ht1632_senddata(i, 0);  // clear the display!
  Serial.println("delay");
  delay(LONGDELAY);
  Serial.println("starting loop");
}

/*
 * we keep a copy of the display controller contents so that we can
 * know which bits are on without having to (slowly) read the device.
 * Note that we only use the low four bits of the shadow ram, since
 * we're shadowing 4-bit memory.  This makes things faster, but we
 * COULD do something with the other half of our bytes !
 */
byte ht1632_shadowram[96];  // our copy of the display's RAM

/*
 * plot a point on the display, with the upper left hand corner
 * being (0,0), and the lower right hand corner being (23, 15).
 * Note that Y increases going "downward" in contrast with most
 * mathematical coordiate systems, but in common with many displays
 * No error checking; bad things may happen if arguments are out of
 * bounds!  (The ASSERTS compile to nothing by default
 */
void plot (char x, char y, char val)
{
  char addr, bitval;

  ASSERT(x >= 0);
  ASSERT(x <= X_MAX);
  ASSERT(y >= 0);
  ASSERT(y <= y_MAX);

  /*
   * The 4 bits in a single memory word go DOWN, with the LSB
   * (last transmitted) bit being on top.  However, writebits()
   * sends the LSB first, so we have to do a sort of bit-reversal
   * somewhere.  Here, this is done by shifting the single bit in
   * the opposite direction from what you might expect.
   */
  bitval = 8>>(y&3);  // compute which bit will need set
  addr = (x<<2) + (y>>2);  // compute which memory word this is in
  if (val) {  // Modify the shadow memory
    ht1632_shadowram[addr] |= bitval;
  }
  else {
    ht1632_shadowram[addr] &= ~bitval;
  }
  // Now copy the new memory value to the display
  ht1632_senddata(addr, ht1632_shadowram[addr]);
}

/*
 * scan()
 * scan all the leds one at a time,
 * using the plot() function, so as to demonstate the limits
 * of "plot" performance.
 */
void scan ()
{
  byte x,y, bits;
  for (y=0; y<=Y_MAX; y++) {
    for (x=0; x <= X_MAX; x++) {
    plot(x,y,1);
    delay(DISPDELAY);
    plot(x,y,0);
    }
  }
}

/*
 * random_walk()
 * have a single LED walk all over the display, randomly.
 */
void random_walk ()
{
  char x,y, dx, dy;
  dx = dy = 1;
  byte change;

  for (int i=0; i < (DEMOTIME/DISPDELAY);  i++) {
    plot(x,y,1);  // draw a point
    delay(DISPDELAY); // wait a bit.
    change = random(1,32);
    /*
     * figure out where to go next.  This code is a bit
     * random in more senses than one, but it seems to
     * have results that are more or less what I had in
     * mind for this portion of the demo.
     */
    if (change < 9) {
    // do nothing
    }
    else if (change == 10) {
    dx = -dx;
    }
    else if (change == 11) {
    dy = -dy;
    }
    else if (change == 12) {
    dx = dy = 0;
    }
    else if (change == 13) {
    dx = dy = 1;
    }
    else if (change == 14) {
    dx = dy = -1;
    }

    plot(x,y,0); // erase the point

    x = x + dx;
    y = y+ dy;
    if (x > X_MAX) {
    dx = -1;
    x = X_MAX;
    }
    else if (x < 0) {
    x = 0;
    dx = 1;
    }
    if (y >  Y_MAX) {
    y = Y_MAX;
    dy = -1;
    }
    else if (y < 0) {
    y = 0;
    dy = 1;
    }
  }
}

/*
 * Draw a line between two points using the bresenham algorithm.
 * This particular bit of code is copied nearly verbatim from
 *   http://www.gamedev.net/reference/articles/article1275.asp
 * I don't like it much (too many local variables!), but it works,
 * and is fully explained at the site, so...
 */
void bres_line(byte x1, byte y1,
unsigned char x2, unsigned char y2,
unsigned char val )
{
  char deltax = abs(x2 - x1);        // The difference between the x's
  char deltay = abs(y2 - y1);        // The difference between the y's
  char x = x1;                       // Start x off at the first pixel
  char y = y1;                       // Start y off at the first pixel
  char xinc1, xinc2, yinc1, yinc2, den, num, numadd, numpixels, curpixel;

  if (x2 >= x1) {                // The x-values are increasing
    xinc1 = 1;
    xinc2 = 1;
  }  
  else {                          // The x-values are decreasing
    xinc1 = -1;
    xinc2 = -1;
  }

  if (y2 >= y1)                 // The y-values are increasing
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          // The y-values are decreasing
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         // There is at least one x-value for every y-value
  {
    xinc1 = 0;                  // Don't change the x when numerator >= denominator
    yinc2 = 0;                  // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         // There are more x-values than y-values
  }
  else                          // There is at least one y-value for every x-value
  {
    xinc2 = 0;                  // Don't change the x for every iteration
    yinc1 = 0;                  // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         // There are more y-values than x-values
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    plot(x, y, val);             // Draw the current pixel
    num += numadd;              // Increase the numerator by the top of the fraction
    if (num >= den)             // Check if numerator >= denominator
    {
      num -= den;               // Calculate the new numerator value
      x += xinc1;               // Change the x as appropriate
      y += yinc1;               // Change the y as appropriate
    }
    x += xinc2;                 // Change the x as appropriate
    y += yinc2;                 // Change the y as appropriate
  }
}

/*
 * Draw a crpss on the display, and then use the PWM function
 * to demonstate how well (or not) the ht1632 dimming function works.
 */
void cross ()
{
  byte x, y;
  char intensity;
  for (x=0; x <= X_MAX; x++) {
    plot(x,7,1);
  }
  for (y=0; y < Y_MAX; y++) {
    plot(12,y,1);
  }
  for (intensity=14; intensity >= 0; intensity--) {
    ht1632_sendcmd(HT1632_CMD_PWM + intensity);
    delay(LONGDELAY/4);
  }
  ht1632_sendcmd(HT1632_CMD_PWM + 15); // back to max brightness
}

/*
 * bouncyline
 * Do the classic "bouncing line" demo, where the endpoints of a line
 * move independently and bounce off the edges of the display.
 * This should demonstrate (more or less) the performance limits of
 * the line drawing function.
 */
void bouncyline ()
{
  char x1,y1, x2,y2, dx1, dy1, dx2, dy2;

  x1 = random(0,X_MAX);
  x2 = random(0,X_MAX);
  y1 = random(0,Y_MAX);
  y2 = random(0,Y_MAX);
  dx1 = random(1,4);
  dx2 = random(1,4);
  dy1 = random(1,4);
  dy2 = random(1,4);
  for (int i=0; i < DEMOTIME/DISPDELAY; i++) {
    bres_line(x1,y1, x2,y2, 1);
    delay(DISPDELAY);
    bres_line(x1,y1, x2,y2, 0);

    x1 += dx1;
    if (x1 > X_MAX) {
      x1 = X_MAX;
      dx1 = -random(1,4);
    }
    else if (x1 < 0) {
      x1 = 0;
      dx1 = random(1,4);
    }

    x2 += dx2;
    if (x2 > X_MAX) {
      x2 = X_MAX;
      dx2 = -random(1,4);
    }
    else if (x2 < 0) {
      x2 = 0;
      dx2 = random(1,4);
    }

    y1 += dy1;
    if (y1 > Y_MAX) {
      y1 = Y_MAX;
      dy1 = -random(1,3);
    }
    else if (y1 < 0) {
      y1 = 0;
      dy1 = random(1,3);
    }

    y2 += dy2;
    if (y2 > Y_MAX) {
      y2 = Y_MAX;
      dy2 = -random(1,3);
    }
    else if (y2 < 0) {
      y2 = 0;
      dy2 = random(1,3);
    }
  }
}


void loop ()
{
  Serial.println("in loop");
  Serial.println("seeding");
  randomSeed(analogRead(0));

  Serial.println("bouncyline");
  bouncyline();
  Serial.println("delay");
  delay(LONGDELAY);
  
  Serial.println("bres_line");
  bres_line(0,0, 16,16, 1);
  Serial.println("delay");
  delay(LONGDELAY);
  
  Serial.println("cross");
  cross();
  Serial.println("delay");
  delay(LONGDELAY);
  
  Serial.println("scan");
  scan();
  Serial.println("delay");
  delay(LONGDELAY);

  Serial.println("random_walk");
  random_walk();

  Serial.println("delay");
  delay(LONGDELAY);
}
