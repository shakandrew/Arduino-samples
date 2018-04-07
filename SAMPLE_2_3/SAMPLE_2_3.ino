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
        B01101111 /* 9 */
};

/*
 * ############################
 * #  TMR2 structure + fns()  #
 * ############################
 *
 * TMR2 struct variable must be volatile for correct work
 */
#define TMR2_SEC_MAX 100
#define TMR2_MILL_MAX 1000

typedef struct {
        short sec;
        short mill;
} TMR2;

typedef struct {
        short led_time;
        byte led_index;
} LED;

void initTMR(volatile TMR2& tmr) {
        tmr.sec = 0;
        tmr.mill = 0;

        TCCR2A |= _BV(WGM21);
        OCR2A = 0xAF;
        TIMSK2 |= _BV(OCIE2A);
        TCCR2B |= _BV(CS22);
}

void incTMR(volatile TMR2& tmr) {
        tmr.mill++;
        tmr.sec += tmr.mill/TMR2_MILL_MAX;
        tmr.mill %= TMR2_MILL_MAX;
        tmr.sec %= TMR2_SEC_MAX;
}

void decTMR(volatile TMR2& tmr) {
        tmr.mill--;
        if (tmr.mill < 0) {
                tmr.mill = TMR2_MILL_MAX + tmr.mill;
                tmr.sec--;
        }
        if (tmr.sec < 0)
                tmr.sec = TMR2_SEC_MAX + tmr.sec;
}

void setTMR(TMR2& tmr, short sec, short mill) {
        tmr.sec = sec;
        tmr.mill = mill;
}

/*
 * ############################
 * #   LED structure and fns  #
 * ############################
 */

void initLED(LED& led){
        led.led_time = 0;
        led.led_index = 0;

        DDRD |= B11110000; // init PD4-PD7 with OUTPUT | segments a,b,c,d
        DDRB |= B00001111; // init PB0-PB3 with OUTPUT | segments e,f,g,dp

        DDRC |= B00001111; // set PC0 - PC3 with OUTPUT | groud for leds 0,1,2,3
}

void updateLEDTime(LED& led, volatile TMR2& tmr) {
        led.led_time = tmr.sec * 100 + tmr.mill / 10;
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

/*
 * ############################
 * #         main code        #
 * ############################
 */


volatile TMR2 tmr;
LED led;

ISR (TIMER2_COMPA_vect) {
        TCNT2 = 0;
        incTMR(tmr);
}

void setup() {
        //Serial.begin(9600);
        initTMR(tmr);
        initLED(led);
        sei();
}

void loop() {
        cli();
        updateLEDTime(led, tmr);
        sei();
        //Serial.println(tmr.sec);
        turnLED(led);
        nextLED(led);
}
