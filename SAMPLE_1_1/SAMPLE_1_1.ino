/* SSD leds - arduino's pins correspondence
 * segment leds    pin  pin on Arduino 
 * a            -> PD4  DIGITAL 4
 * b            -> PD5  DIGITAL 5
 * c            -> PD6  DIGITAL 6
 * d            -> PD7  DIGITAL 7
 * 
 * e            -> PB0  DIGITAL 8
 * f            -> PB1  DIGITAL 9
 * g            -> PB2  DIGITAL 10
 * dot          -> PB3  DIGITAL 11
 *
 * Lets code every numeral into byte format
 * For example - 6:
 *    _a
 * f |_  b  #g in the middle#
 * e |_|.c
 *   d   \ 
 *        dot
 * index: 7   6 5 4 3 2 1 0
 * value: 0   1 1 1 1 1 0 1 
 * char:  dot g f e d c b e
 */
const byte numerals[] = {
                          B00111111, /* 0 */
                          B00000110, /* 1 */
                          B01011011, /* 2 */
                          B01001111, /* 3 */
                          B01100110, /* 4 */
                          B01101101, /* 5 */
                          B01111101, /* 6 */
                          B00000111, /* 7 */
                          B01111111, /* 8 */
                          B01101111  /* 9 */
                        };

void show_LED_numeral(byte);
void turnLight(byte);
const byte NUMERAL_COUNT = 10;
const byte LEDS_COUNT = 8;
const byte BIN128 = B10000000; /* dot */
const byte BIN0 = 0;
const byte PORTD_ZERO_MASK = B00001111;
const byte PORTB_ZERO_MASK = B11110000;
/*
 * Init begginings
 */
void setup() {
  DDRD = DDRD | B11110000;
  DDRB = DDRB | B00001111;
}
/*
 * Infinite loop
 */
void loop() {
  for (byte led_index = 0; led_index < NUMERAL_COUNT; led_index++) {
    show_LED_numeral(numerals[led_index]);
  }
  // to get dot we need to add to every numeral BIN128
  for (byte led_index = NUMERAL_COUNT; led_index > 0; led_index--) {
    show_LED_numeral(numerals[led_index - 1]+BIN128); 
  }
}
/*
 * Show numeral for 1 second
 */
void show_LED_numeral(byte numeral) {
  turnLight(numeral);
  delay(1000);
  turnLight(BIN0);
}
/*
 * Turn ON/OFF lights
 */
void turnLight(byte numeral) {
  PORTD = PORTD&PORTD_ZERO_MASK|(numeral<<4);
  PORTB = PORTB&PORTB_ZERO_MASK|(numeral>>4);
}
