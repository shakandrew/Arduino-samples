
#define TMR2_SEC_MAX 100
#define TMR2_MILL_MAX 1000

typedef struct {
    short sec;
    short mill;
} TMR2;

void initTMR(volatile TMR2& tmr);
void incTMR(volatile TMR2& tmr);
void decTMR(volatile TMR2& tmr);
void setTMR(TMR2& tmr, short sec, short mill);

void initTMR(volatile TMR2& tmr) {
    tmr.sec = 0;
    tmr.mill = 0;

    TCCR2A |= _BV(WGM21);
    OCR2A = 0x10;
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
