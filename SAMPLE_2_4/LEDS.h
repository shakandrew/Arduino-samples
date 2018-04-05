#define PORTD_UPDATE(num) B00001111|(num<<4)
#define PORTC_UPDATE(index) B00001111^(1<<index)
#define PORTB_UPDATE(num) B11110000|(num>>4)


namespace LEDS {

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

// public functions
void init();
void turnLEDOn(byte led_index, byte value);

void init(){
        DDRD |= B11110000; // init PD4-PD7 with OUTPUT | segments a,b,c,d
        DDRB |= B00001111; // init PB0-PB3 with OUTPUT | segments e,f,g,dp
        DDRC |= B00001111; // set PC0 - PC3 with OUTPUT | groud for leds 0,1,2,3
}

void turnLEDOn(byte led_index, byte numeral) {
        if (led_index == 2) numeral += B10000000;
        PORTD = PORTD_UPDATE(numeral);
        PORTB = PORTB_UPDATE(numeral);
        PORTC = PORTC_UPDATE(led_index);
}

}
