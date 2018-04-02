
#define TMR2_SEC_MAX 100
#define TMR2_MILL_MAX 1000

typedef struct {
    short mill;
    short sec;
    short period;
} TMR2;

void initTMR(volatile TMR2& tmr);
void incTMR(volatile TMR2& tmr);
void setTMR(TMR2& tmr, short sec, short mill);

void initTMR(volatile TMR2& tmr) {
    tmr.sec = 0;
    tmr.mill = 0;
    tmr.period = 10;

    TCCR2A |= _BV(WGM21);
    OCR2A = 0x10;
    TIMSK2 |= _BV(OCIE2A);
    TCCR2B |= _BV(CS22);
}

void incTMR(volatile TMR2& tmr) {
    tmr.mill += tmr.period;
    tmr.sec += tmr.mill/TMR2_MILL_MAX;
    tmr.mill %= TMR2_MILL_MAX;
    tmr.sec %= TMR2_SEC_MAX;
    if (tmr.mill < 0)
        tmr.mill = TMR2_MILL_MAX + tmr.mill;
    if (tmr.sec < 0)
        tmr.sec = TMR2_SEC_MAX + tmr.sec;
}

void setTMR(TMR2& tmr, short sec, short mill, short period) {
    tmr.sec = sec;
    tmr.mill = mill;
    tmr.period = period;
}
