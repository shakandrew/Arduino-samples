
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

typedef struct {
    short led_time;
    byte led_index;
} LED;

void initLED(LED& led);
void turnLED(LED& led);
void nextLED(LED& led);
void turnLight(byte led_index, byte numeral);

void initLED(LED& led){
    led.led_time = 0;
    led.led_index = 0;

    DDRD |= B11110000; // init PD4-PD7 with OUTPUT | segments a,b,c,d
    DDRB |= B00001111; // init PB0-PB3 with OUTPUT | segments e,f,g,dp

    DDRC |= B00001111; // set PC0 - PC3 with OUTPUT | groud for leds 0,1,2,3
}

void turnLED(LED& led){
    short temp = led.led_time;
    if (led.led_index == 0) temp /= 1;
    if (led.led_index == 1) temp /= 10;
    if (led.led_index == 2) temp /= 100;
    if (led.led_index == 3) temp /= 1000;
    turnLight(led.led_index, numerals[temp%10]);
}

void nextLED(LED& led){
    led.led_index = (++led.led_index)%4;
}

void turnLight(byte led_index, byte numeral) {
    if (led_index == 2) numeral += B10000000;
    PORTD = PORTD_ZERO_MASK | (numeral << 4);
    PORTB = PORTB_ZERO_MASK | (numeral >> 4);
    PORTC = PORTC_ZERO_MASK ^ _BV(led_index);
}
