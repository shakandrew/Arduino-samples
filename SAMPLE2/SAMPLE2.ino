#define BIN128 B10000000 /* dot */
#define BIN0 B0 /* zero value */
#define PORTD_ZERO_MASK B00001111 /* mask for d port (to left unchanged first 4 bits) */
#define PORTB_ZERO_MASK B11110000 /* The same like with D, but first 4 bits */
#define LED_INDEX_MAX B1001
#define LED_INDEX_MIN B0

#define ON_STR "on\0\0"
#define OFF_STR "off\0"
#define PAUSE_STR "pause\0\0\0"
#define RESET_STR "reset\0\0\0"
#define DELAY_STR "delay\0\0\0"
#define HELP_STR "help\0\0\0\0"
#define EMPTY_VALUE_STR "\0\0\0\0"

#define TITLE_SIZE 8
#define VALUE_SIZE 4

const byte numerals[] = {
        B00111111, /* 0 */
        B00000110, /* 1 */
        B01011011, /* 2 */
        B01001111, /* 3 */
        B01100110, /* 4 */
        B01101101, /* 5 */
        B01111101, /* 6 */
        B00000111, /* 7 */
        B01111111, /* 8 */
        B01101111  /* 9 */
};

typedef enum {
    PAUSE_ON,
    PAUSE_OFF,
    RESET,
    DELAY,
    HELP
} LED_CMD;

typedef struct {
    char title[TITLE_SIZE];
    char value[VALUE_SIZE];
    LED_CMD led_cmd;
} Command;

typedef struct {
    int led_delay;
    byte counter;
    byte led_index;
    bool led_point;
    char changer;
} LED;

// BEGIN FUNCTION DECLARATION

// Hardware
void showLEDNumeral(byte);
void turnLight(byte);

// LED
void resetLED(LED *);
void nextLED(LED *, bool);
void showLED(LED *);
void changeDelayLED(LED *, Command *);

// Command
void resetCommand(Command *);
void initCommand(Command *, byte, char *);

// Serial monitor commands
void checkForCommands();
void errorBadCommand(byte, char *);
void help();

// Checks + work with strigns
void copyWord(byte, char *, byte, char *, byte);
bool compareStr(byte, char *, byte, char *);
bool check_PAUSE_ON(Command *);
bool check_PAUSE_OFF(Command *);
bool check_RESET(Command *);
bool check_DELAY(Command *);
bool check_HELP(Command *);

// END FUNCTION DECLARATION

Command *cmd;
LED *led;

void setup() {
    Serial.begin(9600);

    cmd = (Command *) malloc(sizeof(Command));
    resetCommand(cmd);

    led = (LED *) malloc(sizeof(LED));
    resetLED(led);

    DDRD = DDRD | B11110000; // init PD4-PD7 with OUTPUT
    DDRB = DDRB | B00001111; // init PB0-PB3 with OUTPUT
}

void loop() {
    showLED(led);
    checkForCommands();
    switch (cmd->led_cmd) {
        case PAUSE_ON:
            nextLED(led, false);
            break;
        case PAUSE_OFF:
            nextLED(led, true);
            break;
        case RESET:
            resetLED(led);
            cmd->led_cmd = PAUSE_OFF;
            break;
        case DELAY:
            changeDelayLED(led, cmd);
            cmd->led_cmd = PAUSE_OFF;
            break;
        case HELP:
            help();
            cmd->led_cmd = PAUSE_OFF;
            break;
        default:
            break;
    }
}

/*
 * Hardware functions
 */

void showLEDNumeral(byte numeral) {
    turnLight(numeral);
    delay(led->led_delay);
    turnLight(BIN0);
}

void turnLight(byte numeral) {
    PORTD = PORTD & PORTD_ZERO_MASK | (numeral << 4);
    PORTB = PORTB & PORTB_ZERO_MASK | (numeral >> 4);
}

/*
 * LED functions
 */

void resetLED(LED *led) {
    led->counter = 0;
    led->led_index = 0;
    led->led_point = false;
    led->led_delay = 1000;
    led->changer = 1;
}

void showLED(LED *led) {
    if (!led->led_point)
        showLEDNumeral(numerals[led->led_index]);
    else
        showLEDNumeral(numerals[led->led_index] + BIN128);
}

void nextLED(LED *led, bool pause_off) {
    if ((led->counter == LED_INDEX_MIN && led->changer == -1) || (led->counter == LED_INDEX_MAX && led->changer == 1))
        led->changer *= -1;
    else
        led->counter += led->changer;

    if (pause_off){
        led->led_index = led->counter;
        led->led_point = (led->changer>0)? false : true;
    }
}

void changeDelayLED(LED *led, Command *cmd) {
    int temp = 0;
    for (byte i = 0; cmd->value[i] >= '0' && cmd->value[i] <= '9' && i < VALUE_SIZE; i++) {
        temp = temp * 10 + cmd->value[i] - '0';
    }
    led->led_delay = temp;
}

/*
 * Command functions
 */

void resetCommand(Command *cmd) {
    cmd->led_cmd = PAUSE_OFF;
}

// Don't like this code | need refactoring
void initCommand(Command *cmd, byte n, char *str) {
    for (byte i = 0; i < TITLE_SIZE; i++) cmd->title[i] = '\0';
    for (byte i = 0; i < VALUE_SIZE; i++) cmd->value[i] = '\0';

    for (byte i = 0; i < n && str[i] != '\0' && str[i] != '\n'; i++) {
        if (i < n && str[i] != '\0' && str[i] != '\n' && str[i] != ' ') {
            byte j = i;
            while (j < n && str[j] != '\0' && str[j] != '\n' && str[j] != ' ')
                j++;
            if (cmd->title[0] == '\0')
                copyWord(str, i, j, TITLE_SIZE, cmd->title);
            else
                copyWord(str, i, j, VALUE_SIZE, cmd->value);
            i = j;
        }
    }
    if (check_PAUSE_ON(cmd)) return;
    if (check_PAUSE_OFF(cmd)) return;
    if (check_RESET(cmd)) return;
    if (check_HELP(cmd)) return;
    if (check_DELAY(cmd)) return;
    errorBadCommand(n, str);
}

/*
 * Serial monitor functions
 */

void checkForCommands() {
    if (Serial.available() > 0) {
        String str = Serial.readStringUntil('\n');
        Serial.println("Serial monitor accepted command: " + str);
        initCommand(cmd, (byte) str.length(), str.c_str());
    }
}

void errorBadCommand(byte n, char *str) {
    Serial.print("[ERROR] bad command  \"");
    for (byte i = 0; i < n; i++)
        Serial.print(str[i]);
    Serial.println("\"\nTry \"help\" to get more info");
}

void help() {
    Serial.print("You can use next commands:\n");
    Serial.print("\"pause on\" - to pause(freeze) the display, but let counter count\n");
    Serial.print("\"pause off\" - to start the display, it will show current counter numeral\n");
    Serial.print("\"reset\" - resets counter and numeral show on the display\n");
    Serial.print("\"delay %time\" - (e.g. delay 156) every numeral will be shown during this time (<=9999 work only with first 4 digits)\n");
    Serial.print("\"help\" - show help\n");
}

/*
 * Check functions
 */

bool check_PAUSE_ON(Command *cmd) {
    if (compareStr(TITLE_SIZE, cmd->title, TITLE_SIZE, PAUSE_STR) &&
        compareStr(VALUE_SIZE, cmd->value, VALUE_SIZE, ON_STR)) {
        cmd->led_cmd = PAUSE_ON;
        Serial.println("pause on accepted");
        return true;
    }
    return false;
}

bool check_PAUSE_OFF(Command *cmd) {
    if (compareStr(TITLE_SIZE, cmd->title, TITLE_SIZE, PAUSE_STR) &&
        compareStr(VALUE_SIZE, cmd->value, VALUE_SIZE, OFF_STR)) {
        cmd->led_cmd = PAUSE_OFF;
        Serial.println("pause off accepted");
        return true;
    }
    return false;
}

bool check_RESET(Command *cmd) {
    if (compareStr(TITLE_SIZE, cmd->title, TITLE_SIZE, RESET_STR) &&
        compareStr(VALUE_SIZE, cmd->value, VALUE_SIZE, EMPTY_VALUE_STR)) {
        cmd->led_cmd = RESET;
        Serial.println("check accepted");
        return true;
    }
    return false;
}

bool check_DELAY(Command *cmd) {
    if (compareStr(TITLE_SIZE, cmd->title, TITLE_SIZE, DELAY_STR)) {
        int led_delay;
        for (byte i = 0; i < VALUE_SIZE; i++) {
            if ((cmd->value[i] < '0' || cmd->value[i] > '9') && cmd->value[i]!='\0') return false;
        }
        cmd->led_cmd = DELAY;
        Serial.println("delay accepted");
        return true;
    }
    return false;
}

bool check_HELP(Command *cmd) {
    if (compareStr(TITLE_SIZE, cmd->title, TITLE_SIZE, HELP_STR) &&
        compareStr(VALUE_SIZE, cmd->value, VALUE_SIZE, EMPTY_VALUE_STR)) {
        cmd->led_cmd = HELP;
        Serial.println("help accepted");
        return true;
    }
    return false;
}

/*
 * Str functions
 */

bool compareStr(byte n, char *str1, byte m, char *str2) {
    if (n != m) return false;
    for (byte i = 0; i < n; i++)
        if (str1[i] != str2[i]) {
            return false;
        }
    return true;
}

void copyWord(char *from_str, byte from, byte to, byte n, char *to_str) {
    for (byte i = from, j = 0; i < to; i++, j++) {
        to_str[j] = from_str[i];
        if (to_str[j] >= 'A' && to_str[j] <= 'Z')
            to_str[j] = 'a' + to_str[j] - 'A';
    }
}
