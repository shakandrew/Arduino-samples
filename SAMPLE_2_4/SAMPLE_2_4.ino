#include "TMR2.h"
#include "LEDS.h"
#include "CMD.h"
#define MAX_DISP_VALUE;

typedef struct {
        volatile int period;
        volatile int value;
        byte index;
} Display;

void looper();
void checkForNewCommand(CMD::COMMAND& cmd);
void  pause(CMD::COMMAND&), period(CMD::COMMAND&),
reset(), error(), up(), down(), help();

Display display;
CMD::COMMAND cmd;

void setup() {
        Serial.begin(9600);

        CMD::init(cmd);

        LEDS::init();

        TMR2::init();
        TMR2::set(10, looper);
        TMR2::start();
}
/*
   TODO
 */
void loop() {
        CMD::COMMAND temp;
        CMD::checkForNewCommand(temp);
        if (temp.cmd == PAUSE) pause();
        else if (temp.cmd == RESET) reset();
        else if (temp.cmd == UP) up();
        else if (temp.cmd == DOWN) down();
        else if (temp.cmd == HELP) help();
        else if (temp.cmd == PERIOD) period(temp);
        else error();
}

void looper(){
        display.value = (display.value + display.period) % MAX_DISP_VALUE;
        if (display.value<0)
                display.value = MAX_DISP_VALUE - display.value;
}

void checkForNewCommand(COMMAND& cmd) {
        if (Serial.available() > 0) {
                String str = Serial.readStringUntil('\n');
                Serial.println("Serial monitor accepted command: " + str);
                CMD::checkCommand(cmd, (byte) str.length(), str.c_str());
        }
}

void error() {
        Serial.println("[ERROR]: Bad command.");
        Serial.println("Try \"help\" to get more info");
}

void help() {
        Serial.println("You can use next commands:");
        Serial.println("\"pause on\" - to pause(freeze) the display, but let counter count");
        Serial.println("\"pause off\" - to start the display, it will show current counter numeral");
        Serial.println("\"up\" - to count incrementing");
        Serial.println("\"down\" - to count decrementing");
        Serial.println("\"period XXXX\" - set period of refreshing the screen to XXXX\nEx \"period 1234\" - sets period 12 sec 340 mills");
        Serial.println("\"reset\" - resets counter and numeral show on the display");
        Serial.println("\"help\" - show help");
}
