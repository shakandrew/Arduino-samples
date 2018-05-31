/* frequency_counter.ino
 * This is a frequency counter using Timer 1 Input Capture
 * author: Antonio C. DomГ­nguez Brito <adominguez@iusiani.ulpgc.es>
 *
 * Description: this program measures the period of a periodic digital signal,
 * for example, a digital clock, or a PWM signal. It uses the input capture
 * facility provided by TIMER1
 */

#include <Arduino.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
/*
 * TEST_INPUT_CAPTURE: this macro constant allows to test the frequency_counter
 * on the same Arduino UNO board. In this case, it uses TIMER1_OVERFLOW_PIN to
 * generate a digital periodic signal. This pin happens to be pin 13, where is
 * the Arduino's led, so you will see a blinking led at a specific period. Keep
 * this constant commented if you do not want to test this program on the same
 * Arduino board.
 */
//#define TEST_INPUT_CAPTURE

#define OC2A_PIN 11

#ifdef TEST_INPUT_CAPTURE
const int TIMER1_OVERFLOW_PIN = 13;
#define TIMER1_OVERFLOW_PIN_PORT PORTB // Arduino's pin 13
const int TIMER1_OVERFLOW_PIN_BIT = 5;

/*
     * Constant TIMER1_OVERFLOW_COUNTS determines the period of the digital
     * test signal. It is a multiple of the TIMER1 clock prescaler period the
     * TOP value set for TIMER1.
     */
const int TIMER1_OVERFLOW_COUNTS = 100; // aproximately 409.6 msecs (100*4.096 msecs.)

volatile bool timer1_overflow_pin_status = false;
volatile int timer1_overflow_counter = 0;
#endif // TEST_INPUT_CAPTURE

/*
 * Constant INPUT_CAPTURE_PIN specifies the input capture pin, which in Arduino
 * UNO board is pin 2. In this pin it is where the input signal to measure should
 * be set.
 */
const int INPUT_CAPTURE_PIN = 2;
#define INPUT_CAPTURE_PIN_PORT PORTD // Arduino's pin 2
const int INPUT_CAPTURE_PIN_BIT = 2;
volatile bool input_capture_pin_status = false;

#define TIMER1_OVERFLOW_VALUE ((uint64_t)(1 << 10))
#define TIMER1_OVERFLOW_HALF_VALUE (1 << 9)

volatile uint64_t timer1_overflows = 0;

/*
 * Macro constant TIMESTAMPS_LENGTH is the length of an internal buffer of capture
 * measurements. The application fills this buffer with TIMESTAMPS_LENGTH captures,
 * and them shows the times collected between captures (which should be the period
 * of the signal) through the Serial interface
 */
#define TIMESTAMPS_LENGTH 20 + 1 // should be less than 256
#define M_PI 3.1416
#define SECS_PER_TIMER1_TICK (double(4e-6))
#define SECS_FRACTIONAL_DIGITS 6
#define HZ_FRACTIONAL_DIGITS 6

volatile uint64_t timestamps[TIMESTAMPS_LENGTH];
volatile uint8_t current_timestamp = 0;
volatile int32_t period=1000;
// interrupt vector for timer1 overflow event
ISR(TIMER1_OVF_vect)
{
    timer1_overflows += TIMER1_OVERFLOW_VALUE;

    uint64_t to_duty = 2500 + (2500 * sin(((2 * M_PI) * timer1_overflows) / period));
    to_duty = map(to_duty, 0, 5000, 0, 254);
    OCR2A = (byte)to_duty;

#ifdef TEST_INPUT_CAPTURE
    timer1_overflow_counter++;
    if (timer1_overflow_counter == TIMER1_OVERFLOW_COUNTS) {
        timer1_overflow_counter = 0;

        // toggle timer1 overflow pin
        timer1_overflow_pin_status = !timer1_overflow_pin_status;
        if (timer1_overflow_pin_status)
            bitSet(TIMER1_OVERFLOW_PIN_PORT, TIMER1_OVERFLOW_PIN_BIT);
        else
            bitClear(TIMER1_OVERFLOW_PIN_PORT, TIMER1_OVERFLOW_PIN_BIT);
    }
#endif // TEST_INPUT_CAPTURE
}
byte duty;
bool mode = true;
// interrupt vector for Timer1 Input Capture event
ISR(TIMER1_CAPT_vect)
{
    uint16_t icr1_register = ICR1;
    if (mode) {
        TCCR1B &= ~_BV(ICES1);
    }
    else {
        TCCR1B |= _BV(ICES1);
    }
    mode = !mode;
    TIFR1 |= _BV(ICF1);

    if (current_timestamp < TIMESTAMPS_LENGTH) {
        timestamps[current_timestamp] = timer1_overflows + icr1_register;

        if (
            (TIFR1 & (1 << TOV1)) && // is timer1 overflow flag set? and
            (icr1_register < TIMER1_OVERFLOW_HALF_VALUE) // did the capture take place after the overflow?
            )
            timestamps[current_timestamp] += TIMER1_OVERFLOW_VALUE;

        current_timestamp++;
    }

    // toggle timer1 input capture pin
    input_capture_pin_status = !input_capture_pin_status;
    if (input_capture_pin_status)
        bitSet(INPUT_CAPTURE_PIN_PORT, INPUT_CAPTURE_PIN_BIT);
    else
        bitClear(INPUT_CAPTURE_PIN_PORT, INPUT_CAPTURE_PIN_BIT);
}

// function prototypes
void print_uint64_t(uint64_t* value);
void continue_info();
// the setup routine runs once when you press reset:
void setup()
{
#ifdef TEST_INPUT_CAPTURE
    // initialize timer1 overflow pin as output
    pinMode(TIMER1_OVERFLOW_PIN, OUTPUT);
    bitClear(TIMER1_OVERFLOW_PIN_PORT, TIMER1_OVERFLOW_PIN_BIT);
#endif // TEST_INPUT_CAPTURE

    // initialize timer1 input capture pin
    pinMode(INPUT_CAPTURE_PIN, OUTPUT);
    bitClear(INPUT_CAPTURE_PIN_PORT, INPUT_CAPTURE_PIN_BIT);
    Serial.begin(9600);
    //initialize the serial port
    Serial.println(F("----> timer1 registers"));
    Serial.print(F(" TCCR1A="));
    Serial.println(TCCR1A, BIN);
    Serial.print(F(" TCCR1B="));
    Serial.println(TCCR1B, BIN);
    Serial.print(F(" TCCR1C="));
    Serial.println(TCCR1C, BIN);
    Serial.print(F(" TIMSK1="));
    Serial.println(TIMSK1, BIN);
    Serial.print(F(" TIFR1="));
    Serial.println(TIFR1, BIN);

    Serial.println("----> setting timer1 registers");
    // Timer1 Input Capture settings:
    //   - activating the noise canceller (implies a 4-oscillator-cycle delay)
    //   - setting falling edge for input capture signal
    //   - setting fast 10-bit pwm mode
    //     (supposing a prescaler of 64 -> T=4 usecs)
    TCCR1A |= (1 << (WGM11)) | (1 << (WGM10));
    Serial.print(F(" TCCR1A="));
    Serial.println(TCCR1A, BIN);
    //TCCR1B |= (1<<(ICNC1)) | (1<<(ICES1)) | (1<<(WGM12));
    TCCR1B |= (1 << (ICNC1)) | (1 << (WGM12)) | (1 << (CS11)) | (1 << (CS10));
    TCCR1B &= ~(1 << (CS12));
    TCCR1B &= ~(1 << (ICES1)) & ~(1 << (WGM13));
    Serial.print(" TCCR1B=");
    Serial.println(TCCR1B, BIN);
    //   - enabling input capture interrupt
    //   - enabling timer1 overflow interrupt
    TIMSK1 |= (1 << (ICIE1)) | (1 << (TOIE1));
    Serial.print(F(" TIMSK1="));
    Serial.println(TIMSK1, BIN);

#ifdef OC2A_PIN
    pinMode(OC2A_PIN, OUTPUT);
    Serial.println(F("----> timer2 registers (starting)"));
    Serial.print(F(" TCCR2A="));
    Serial.println(TCCR2A, BIN);
    Serial.print(F(" TCCR2B="));
    Serial.println(TCCR2B, BIN);
    Serial.print(F(" TIMSK2="));
    Serial.println(TIMSK2, BIN);
    Serial.print(F(" TIFR2="));
    Serial.println(TIFR2, BIN);

    Serial.println("----> setting timer2 registers");
    // Timer2settings:
    //   - fast pwm with max on 0xff
    //   - prescaler of 64
    //   - OC2A pwm not inverted output
    //   - OC2B pwm not inverted output
    TCCR2A = 0x0;
    TCCR2A |= (1 << (WGM21)) | (1 << (WGM20));
    TCCR2A |= (1 << (COM2A1)) | (1 << (COM2B1));
    Serial.print(F(" TCCR2A="));
    Serial.println(TCCR2A, BIN);
    TCCR2B &= ~(1 << (WGM22)) & ~(1 << (CS21)) & ~(1 << (CS20));
    TCCR2B |= (1 << (CS22));
    Serial.print(F(" TCCR2B="));
    Serial.println(TCCR2B, BIN);
    // stablishing a duty cycle of 50%
    OCR2A = 127; // this is Arduino UNO pin 11
#endif

    Serial.println(F("----> capturing signal ..."));

    interrupts();
}

// the loop routine runs over and over again forever:
void loop()
{
    if (current_timestamp == TIMESTAMPS_LENGTH) {
        Serial.print(F("----> "));
        Serial.print(TIMESTAMPS_LENGTH);
        Serial.println(F(" timestamps taken"));

        uint64_t previous = *timestamps, current;
        for (int i = 1; i < TIMESTAMPS_LENGTH; i++) {
            current = timestamps[i];
            uint64_t difference = current - previous;
            double difference_in_seconds = double(difference) * SECS_PER_TIMER1_TICK;
            double frequency = 1 / difference_in_seconds;

            Serial.print(F("  "));
            Serial.print(i);
            Serial.print(F(": "));
            print_uint64_t(&current);
            Serial.print('-');
            print_uint64_t(&previous);
            Serial.print('=');
            print_uint64_t(&difference);
            Serial.print("(");
            Serial.print(difference_in_seconds, SECS_FRACTIONAL_DIGITS);
            Serial.print(F(" s. <-> "));
            Serial.print(frequency, HZ_FRACTIONAL_DIGITS);
            Serial.println(F(" Hz)"));
            previous = current;
        }
        continue_info();
        String cmd = "";
        while ((cmd != "y") && (cmd != "Y")) {
            if (Serial.available() > 0) {
                cmd = Serial.readString();

                Serial.print(F("Accepted string :"));
                Serial.println(cmd);

                int space_pos = cmd.indexOf(' ');
                String command = cmd.substring(0, space_pos);
                int value = cmd.substring(space_pos + 1, cmd.length()).toInt();
;
                if (command == F("duty") && value>=0 && value<=255) {
                    OCR2A = value;
                    Serial.print(F("Executed: "));
                    Serial.println(cmd);
                } else if (command == F("prescaler") && (value == 0 || value == 1 || value == 8 || value == 32
                                                     || value == 64 || value == 128 || value == 256 || value == 1024)) {
                    // ~(_BV(VAL)): if VAL = 2 -> 00000100 -> 11111011
                    cli();
                    TCCR2B |= (_BV(CS12) | _BV(CS11) | _BV(CS10));
                    if (value == 0) {
                        TCCR2B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
                    }
                    else if (value == 1) {
                        TCCR2B &= ~(_BV(CS12) | _BV(CS11));
                    }
                    else if (value == 8) {
                        TCCR2B &= ~(_BV(CS12) | _BV(CS10));
                    }
                    else if (value == 32) {
                        TCCR2B &= ~_BV(CS12);
                    }
                    else if (value == 64) {
                        TCCR2B &= ~(_BV(CS11) | _BV(CS10));
                    }
                    else if (value == 128) {
                        TCCR2B &= ~_BV(CS11);
                    }
                    else if (value == 256) {
                        TCCR2B &= ~_BV(CS10);
                    }
                    else if (value == 1024) {
                        TCCR2B &= ~0;
                    }
                    sei();
                    Serial.print(F("Executed: "));
                    Serial.println(cmd);
                }
                else if (command == F("voltage") && value>=0 && value <=5000 ) {
                    OCR2A = map(value, 0, 5000, 0, 255);
                    Serial.print(F("Executed: "));
                    Serial.println(cmd);
                }
                else if (command == F("period") && value>=0 && value <=100000) {
                    period = value;
                    Serial.print(F("Executed: "));
                    Serial.println(cmd);
                }
                else if (cmd != "y" ||cmd != "Y"){
                    Serial.println(F("Try once again!"));
                    continue_info();
                }
            }
        }
        current_timestamp = 0;
        Serial.println(F("----> capturing signal ..."));
    }
}

void continue_info()
{
    Serial.println(F("----> continue? "));
    Serial.println(F("*To continue write 'y' or 'Y'"));
    Serial.println(F("* To change duty write 'duty VALUE', where VALUE can be in diaposon from 0 to 255'"));
    Serial.println(F("* To change voltage write 'voltage VALUE', where VALUE can be in diaposon from 0 to 5000'"));
    Serial.println(F("* To change period write 'period VALUE', where VALUE can be in diaposon from 0 to 1000'"));
    Serial.println(F("* To change prescaler write 'prescaler VALUE', where VALUE can be:'"));
    Serial.println(F("\t\t0, 1, 8, 32, 64, 128, 256, 1024"));
}

void print_uint64_t(uint64_t* value)
{
    Serial.print("[");
    Serial.print(uint32_t((*value) >> 32), HEX);
    Serial.print("][");
    Serial.print(uint32_t(*value), HEX);
    Serial.print("]");
}
