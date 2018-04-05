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

int temp_period=0;
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
void loop() {
        CMD::COMMAND temp;
        CMD::checkForNewCommand(temp);
        if (temp.cmd == CMD::PAUSE) pause(temp);
        else if (temp.cmd == CMD::RESET) reset();
        else if (temp.cmd == CMD::UP) up();
        else if (temp.cmd == CMD::DOWN) down();
        else if (temp.cmd == CMD::HELP) help();
        else if (temp.cmd == CMD::PERIOD) period(temp);
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

void pause(CMD::COMMAND& arg){
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
        cmd.cmd = CMD::PAUSE;
        cmd.value = 0;
}
void up() {
        display.value = abs(display.value);
        cmd.cmd = CMD::PAUSE;
        cmd.value = 0;
}
void down() {
        display.value = -abs(display.value);
        cmd.cmd = CMD::PAUSE;
        cmd.value = 0;
}
void period(CMD::COMMAND& arg) {
        display.value = arg.value;
        CMD::set(display.value, looper);
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
