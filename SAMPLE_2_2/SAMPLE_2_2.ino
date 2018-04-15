#include <FlexiTimer2.h>

#define PORTB_ZERO_MASK B11110000 /* The same like with D, but first 4 bits */
#define PORTC_ZERO_MASK B11111111
#define PORTD_ZERO_MASK B00001111 /* mask for d port (to left unchanged first 4 bits) */

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

volatile short led_value = 0;
byte led_index=0;

void timer_up() {
    led_value = ++led_value % 10000;
}

void setup() {
    Serial.begin(9600);
    DDRD |= B11110000; // init PD4-PD7 with OUTPUT
    DDRC |= B00001111; // set PC0 - PC3 with OUTPUT
    DDRB |= B00001111; // init PB0-PB3 with OUTPUT

    FlexiTimer2::set(10, timer_up);
    FlexiTimer2::start();
}

void loop() {
    short temp = led_value;
    if (led_index == 0) temp /= 1;
    if (led_index == 1) temp /= 10;
    if (led_index == 2) temp /= 100;
    if (led_index == 3) temp /= 1000;
    turnLight(led_index, numerals[temp%10]);
    led_index = ++led_index % 4;
}

void turnLight(byte led_index, byte numeral) {
    if (led_index==2) numeral += B10000000;
    PORTD = PORTD_ZERO_MASK | (numeral << 4);
    PORTB = PORTB_ZERO_MASK | (numeral >> 4);
    PORTC = PORTC_ZERO_MASK ^ _BV(led_index);
}
