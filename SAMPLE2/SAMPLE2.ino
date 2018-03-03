#define BIN128 B10000000 /* dot */
#define BIN0 B0 /* zero value */
#define PORTD_ZERO_MASK B00001111 /* mask for d port (to left unchanged first 4 bits) */
#define PORTB_ZERO_MASK B11110000 /* The same like with D, but first 4 bits */
#define LED_INDEX_MAX B1001
#define LED_INDEX_MIN B0
#define LEDS_NUMBER B1000

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

void showLEDNumeral(byte);
void turnLight(byte);
void resetCommand();
void checkCommands();
void errorBadCommand(String);

/*
 * 1 - goes on
 * 2 - paused
 * 3 - reset
 */
byte led_status;
byte led_index;
byte counter;
char led_index_changer;

void setup() {
    Serial.begin(9600);
    resetCommand();
    DDRD = DDRD | B11110000; // init PD4-PD7 with OUTPUT
    DDRB = DDRB | B00001111; // init PB0-PB3 with OUTPUT
}

void loop() {
    if (led_index_changer > 0)
        showLEDNumeral(numerals[led_index]);
    else
        showLEDNumeral(numerals[led_index] + BIN128);
    checkCommands();
    if (led_status != 3) {
        if ((counter == LED_INDEX_MIN && led_index_changer == -1)||(counter == LED_INDEX_MAX && led_index_changer == 1))
            led_index_changer *= -1;
        else
            counter += led_index_changer;

        if (led_status == 1)
            led_index = counter;
    }
    else {
        resetCommand();
    }

    
}

void checkCommands() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toLowerCase();
        Serial.print("Accepted command \"" + command + "\"\n");
        if (command == "pause off") {led_status = 1;return;}
        if (command == "pause on") {led_status = 2;return;}
        if (command == "reset") {led_status = 3;return;}
        errorBadCommand(command);
    }
}

void errorBadCommand(String error) {
    Serial.print("[ERROR] bad command \"" + error + "\"\n");
}

void resetCommand() {
    led_status = 1;
    counter = 0;
    led_index = 0;
    led_index_changer = 1;
}

void showLEDNumeral(byte numeral) {
    turnLight(numeral);
    delay(1000);
    turnLight(BIN0);
}
 
void turnLight(byte numeral) {
    PORTD = PORTD&PORTD_ZERO_MASK|(numeral<<4);
    PORTB = PORTB&PORTB_ZERO_MASK|(numeral>>4);
}

