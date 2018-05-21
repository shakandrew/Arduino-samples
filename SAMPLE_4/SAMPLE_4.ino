#include <Time.h>
#include <TimeLib.h>
#include <stdint.h>
#include <LCD03.h>
#include "MOVE_RD02.h"
#include "TMR1.h"

#define M_PI 3.14159265
#define RD02_I2C_ADDRESS byte((0xB0) >> 1)
#define REFRESH_LIMIT 3000
#define WHEEL_RADIUS 0.048
#define TO_RADS(n) (n * M_PI / 180)
#define timer_PERIOD 1
#define CALL_FREQUENCY (double)timer_PERIOD / 1000
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

MOVE_RD02 motor(RD02_I2C_ADDRESS);

LCD03 lcd;
uint16_t refresh_lcd = 0;

volatile int32_t m1_ticks_prev;
volatile int32_t m1_ticks_curr;

volatile int32_t m2_ticks_prev;
volatile int32_t m2_ticks_curr;

char lcd_display_msg[32] =
    { 'M', '1', ':', ' ', '.', ' ', ' ', ' ', 'H', 'H', ':', 'M', 'M', ':', 'S', 'S',
      'M', '2', ':', ' ', '.', ' ', ' ', ' ', 'D', 'D', '/', 'M', 'M', '/', 'Y', 'Y' };

volatile uint32_t timer;
uint32_t timer_prev;
uint32_t timer_curr;

void looper() {
    timer++;
}

void setup()
{
    Serial.begin(9600);

    // init lcd
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.print("Starting....");
    Serial.println("Please set currect time in format:\n");
    Serial.println("T<Unix timestamp>, ex: T1234 \n");

    delay(3000);

    // Clear the LCD
    lcd.clear();
    lcd.print("Prease, send the time to Arduino");

    m1_ticks_curr = 0;
    m2_ticks_curr = 0;
    timer_curr = 0;
    timer = 0;
    // lcd cant work in interuptions
    TMR1::init();
    TMR1::set(timer_PERIOD, looper);
    TMR1::start();
}

void loop()
{
    if (timeStatus() == timeNotSet)
    {
        if (Serial.available())
            processSyncMessage();
    }
    else
    {
        if (Serial.available())
            command();
        else
            refreshData();
    }
}

void refreshData()
{
    refresh_lcd = (refresh_lcd + 1) % REFRESH_LIMIT;
    if (refresh_lcd == 0)
    {
        {
            timer_prev = timer_curr;
            timer_curr = timer;

            m1_ticks_prev = m1_ticks_curr;
            m1_ticks_curr = motor.getENC1();

            m2_ticks_prev = m2_ticks_curr;
            m2_ticks_curr = motor.getENC2();
        }
        {
            float speed1 = getSpeed( MAX(m1_ticks_prev, m1_ticks_curr), MIN(m1_ticks_prev, m1_ticks_curr) );
            float speed2 = getSpeed( MAX(m2_ticks_prev, m2_ticks_curr), MIN(m2_ticks_prev, m2_ticks_curr) );

            if (m1_ticks_curr>m1_ticks_prev)
                lcd_display_msg[3] = '+';
            if (m1_ticks_curr<m1_ticks_prev)
                lcd_display_msg[3] = '-';
            if (m1_ticks_curr==m1_ticks_prev)
                lcd_display_msg[3] = '0';

            lcd_display_msg[5] = getDigitFromDouble(speed1, -1);
            lcd_display_msg[6] = getDigitFromDouble(speed1, -2);

            lcd_display_msg[8] = '0' + hour() / 10;
            lcd_display_msg[9] = '0' + hour() % 10;

            lcd_display_msg[11] = '0' + minute() / 10;
            lcd_display_msg[12] = '0' + minute() % 10;

            lcd_display_msg[14] = '0' + second() / 10;
            lcd_display_msg[15] = '0' + second() % 10;

            if (m2_ticks_curr>m2_ticks_prev)
                lcd_display_msg[19] = '+';
            if (m2_ticks_curr<m2_ticks_prev)
                lcd_display_msg[19] = '-';
            if (m2_ticks_curr==m2_ticks_prev)
                lcd_display_msg[19] = '0';

            lcd_display_msg[21] = getDigitFromDouble(speed2, -1);
            lcd_display_msg[22] = getDigitFromDouble(speed2, -2);

            lcd_display_msg[24] = '0' + day() / 10;
            lcd_display_msg[25] = '0' + day() % 10;

            lcd_display_msg[27] = '0' + month() / 10;
            lcd_display_msg[28] = '0' + month() % 10;

            lcd_display_msg[30] = '0' + (year() / 10) % 10;
            lcd_display_msg[31] = '0' + year() % 10;

            lcd.print(lcd_display_msg);
        }
    }
}

void processSyncMessage()
{
    char c = Serial.read();
    if (c != 'T')
    {
        Serial.println("Still waiting for setting time.");
        Serial.println("Please set currect time in format:");
        Serial.println("T<Unix timestamp>, ex: T1234");
        return;
    }
    String timestamp = Serial.readStringUntil('\n');
    time_t pctime = 0;
    for (int i = 0; i < timestamp.length(); i++)
    {
        if (timestamp[i] >= '0' && timestamp[i] <= '9')
        {
            pctime = (10 * pctime) + (timestamp[i] - '0'); // convert digits to a number
        }
    }
    setTime(pctime); // Sync Arduino clock to the time received on the serial port

    lcd.home();
    lcd.print(lcd_display_msg);
}

void command()
{
        String temp = Serial.readStringUntil('\n');
        const char* str = temp.c_str();
        Serial.print("Accepted string: ");
        Serial.println(str);

        if (strcmp(str, "forward\0") == 0)
        {
            motor.moveForward();
        }
        if (strcmp(str, "backward\0") == 0)
        {
            motor.moveBackward();
        }
        if (strcmp(str, "left\0") == 0)
        {
            motor.turnLeft();
        }
        if (strcmp(str, "right\0") == 0)
        {
            motor.turnRigth();
        }
        if (strcmp(str, "spinleft\0") == 0)
        {
            motor.spinLeft();
        }
        if (strcmp(str, "spinright\0") == 0)
        {
            motor.spinRigth();
        }
        if (strcmp(str, "stop\0") == 0)
        {
            motor.stopRD();
        }/*
        if(strcmp(str, "speed1\0") == 0){
            Serial.println(motor.getSpeed1());
        }
        if(strcmp(str, "speed2\0") == 0){
          Serial.println(motor.getSpeed2());
        }
        if(strcmp(str, "mode\0") == 0){
          motor.writeRegister(0x2, 0xF);
        }
        if(strcmp(str, "read0\0") == 0){

          byte ret = motor.readRegister(0);
          Serial.println(ret);

        }
        if(strcmp(str, "read1\0") == 0){

          byte ret = motor.readRegister(1);
          Serial.println(ret);

        }
        if(strcmp(str, "read13\0") == 0){

          byte ret = motor.readRegister(13);
          Serial.println(ret);

        }
        if(strcmp(str, "read15\0") == 0){

          byte ret = motor.readRegister(15);
          Serial.println(ret);
        }
        if (strcmp(str, "data\0") == 0)
        {
            Serial.print("timer_prev: ");
            Serial.println(timer_prev);
            Serial.print("timer_curr: ");
            Serial.println(timer_curr);

            Serial.print("m1_ticks_prev: ");
            Serial.println(m1_ticks_prev);
            Serial.print("m1_ticks_curr: ");
            Serial.println(m1_ticks_curr);
            Serial.print("motor.getENC1(): ");
            Serial.println(motor.getENC1());
        }
        */
}

float getSpeed(int32_t bigger, int32_t smaller)
{
    float delta_ts = (timer_curr - timer_prev) * CALL_FREQUENCY;

    float delta = 0;

    if ( ( (uint32_t)bigger - (uint32_t)smaller) > INT32_MAX )
        delta = (INT32_MAX - bigger) + (smaller - INT32_MIN);
    else
        delta = bigger - smaller;
    return (TO_RADS(delta) / delta_ts) * WHEEL_RADIUS;
}


int power(int num, int num_power)
{
    int ans = 1;
    for (int i = 0; i < num_power; i++)
        ans *= num;
    return ans;
}

char getDigitFromDouble(double a, int pos)
{
    char ans = '0';
    if (pos >= 0)
    {
        ans += ((int)(a / power(10, pos))) % 10;
    }
    else
    {
        ans += ((int)(a * power(10, abs(pos)))) % 10;
    }
    return ans;
}
