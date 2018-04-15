#include "TMR2.h"
#include "LEDS.h"
#include "CMD.h"
#include <avr/pgmspace.h>
#define MAX_DISP_VALUE 100000
#define DEFAULT_PERIOD 500

typedef struct {
    volatile long int period;
    volatile long int value;
} Display;

// Functions
void looper(),
     checkForNewCommand(CMD::COMMAND& cmd),
     pause(CMD::COMMAND&),
     period(CMD::COMMAND&),
     reset(),
     error(),
     up(),
     down(),
     help();

// Variables
long int temp_period = 0;
Display display = {DEFAULT_PERIOD, 0};
CMD::COMMAND cmd = {0, CMD::PAUSE};
byte led_index = 0;

void setup() {
    Serial.begin(9600);

    LEDS::init();

    TMR2::init();
    TMR2::set(DEFAULT_PERIOD, looper);
    TMR2::start();

    help();
}
void loop() {

    CMD::COMMAND temp;
    checkForNewCommand(temp);
    if (temp.cmd == CMD::PAUSE) pause(temp);
    else if (temp.cmd == CMD::RESET) reset();
    else if (temp.cmd == CMD::UP) up();
    else if (temp.cmd == CMD::DOWN) down();
    else if (temp.cmd == CMD::HELP) help();
    else if (temp.cmd == CMD::PERIOD) period(temp);
    else error();

    byte val;
    if (led_index == 0) val = (display.value / 10) % 10;
    if (led_index == 1) val = (display.value / 100) % 10;
    if (led_index == 2) val = (display.value / 1000) % 10;
    if (led_index == 3) val = (display.value / 10000) % 10;
    LEDS::turnLEDOn(led_index, val);
    led_index = ++led_index % 4;
}

void looper() {
    display.value = (display.value + display.period) % MAX_DISP_VALUE;
    if (display.value < 0)
        display.value = MAX_DISP_VALUE + display.value;
}

void checkForNewCommand(CMD::COMMAND& arg) {
    if (Serial.available() > 0) {
        String str = Serial.readStringUntil('\n');
        Serial.print(F("Serial monitor accepted command: "));
        Serial.println(str);
        CMD::checkCommand(arg, (byte)str.length(), str.c_str());
    }
    else {
        arg.value = cmd.value;
        arg.cmd = cmd.cmd;
    }
}

void pause(CMD::COMMAND& arg) {
    if (arg.value && !cmd.value) {
        temp_period = display.period;
        display.period = 0;
    }
    if (!arg.value && cmd.value) {
        display.period = temp_period;
    }
    cmd = arg;
}
void reset() {
    display.value = 0;
    display.period = DEFAULT_PERIOD;
    cmd.cmd = CMD::PAUSE;
    cmd.value = 0;
}
void up() {
    display.period = abs(display.period);
    temp_period = abs(temp_period);
}
void down() {
    display.period = -abs(display.period);
    temp_period = -abs(temp_period);
}
void period(CMD::COMMAND& arg) {
    display.period = arg.value;
    TMR2::set(display.period, looper);
}
void error() {
    Serial.println(F("[ERROR]: Bad command."));
    Serial.println(F("Try \"help\" to get more info"));
}

void help() {
    Serial.println(F("You can use next commands:"));
    Serial.println(F("\"pause on\" - to pause(freeze) the display, but let counter count"));
    Serial.println(F("\"pause off\" - to start the display, it will show current counter numeral"));
    Serial.println(F("\"up\" - to count incrementing"));
    Serial.println(F("\"down\" - to count decrementing"));
    Serial.println(F("\"period XXXX\" - set period of refreshing the screen to XXXX\nEx \"period 1234\" - sets period 12 sec 340 mills"));
    Serial.println(F("\"reset\" - resets counter and numeral show on the display"));
    Serial.println(F("\"help\" - show help"));
}
