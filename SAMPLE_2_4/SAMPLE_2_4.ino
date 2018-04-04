#include "timer2.h"
#include "led_display.h"
#include "commands.h"

LED led;
CMD cmd;

void setup() {
    Serial.begin(9600);
    initCMD(cmd);
    initLED(led);
    sei();
}
/* TODO
  period must change led values

*/
void loop() {
    CMD temp;
    checkForNewCommand(temp);
    switch (temp.cmd) {
        case PAUSE:
            cmd = temp;
            if (!cmd.value) {
              cli();
              updateLEDTime(led, tmr);
              sei();
            }
        case RESET:
            cmd = temp;
            setTMR(tmr, 0, 0, 10)
        case UP:

        case DOWN:

        case PERIOD:

        case HELP:
            help();
        case ERROR:
            errorBadCommand();
    }
    turnLED(led);
    nextLED(led);
}


void checkForNewCommand(CMD& cmd) {
    if (Serial.available() > 0) {
        String str = Serial.readStringUntil('\n');
        Serial.println("Serial monitor accepted command: " + str);
        initCommand(cmd, (byte) str.length(), str.c_str());
    }
}

void errorBadCommand() {
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
