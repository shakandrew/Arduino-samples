namespace TMR1 {
volatile long int TPL;     // Timer period limit
volatile long int TPV;     // Timer period value
void (*action)() = NULL;

// public functions
void set(short period, void (*f)());
void start();
void finish();
void init();
// private functions
void inc();

void start() {
    if (action != NULL) {
        TCNT1L = 0;
        TCNT1H = 0;
        sei();
    }
}

void finish() {
    cli();
}

void init() {
    TCCR1A |= _BV(WGM12); // Set CTC mode
    OCR1AL = 0xF9; // Interuption value
    OCR1AH = 0x0;
    TIMSK1 |= _BV(OCIE1A); // Allow interuptions, when TCNT2 == OCR2A
    TCCR1B |= _BV(CS11) | _BV(CS10); // Set prescaler for 64
}

void set(short period, void (*f)()) {
    TPV = 0;
    TPL = period;
    action = f;
}

void inc() {
    TPV ++;
    if (TPV == TPL) {
        TPV = 0;
        action();
    }

}
}

ISR(TIMER1_COMPA_vect) {
    TCNT1L = 0;
    TCNT1H = 0;
    TMR1::inc();
}
