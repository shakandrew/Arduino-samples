#include "timer2.h"
#include "led_display.h"
#include "commands.h"

volatile TMR2 tmr;
LED led;
CMD cmd;

ISR (TIMER2_COMPA_vect) {
    incTMR(tmr);
}

void setup() {
    initCMD(cmd);
    initTMR(tmr);
    initLED(led);
    sei();
}

void loop() {
    checkForNewCommand(cmd);
    // TODO Add switch 
    cli();
    updateLEDTime(led, tmr);
    sei();

    turnLED(led);
    nextLED(led);
}
