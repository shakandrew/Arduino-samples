namespace TMR2 {

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
        TCNT2 = 0;
        sei();
    }
}

void finish() {
    cli();
}

void init() {
    TCCR2A |= _BV(WGM21); // Set CTC mode
    OCR2A = 0xF9; // Interuption value
    TIMSK2 |= _BV(OCIE2A); // Allow interuptions, when TCNT2 == OCR2A
    TCCR2B |= _BV(CS22); // Set prescaler for 64
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

ISR(TIMER2_COMPA_vect) {
    TCNT2 = 0;
    TMR2::inc();
}
