//#include <rd02.h>

#include <Time.h>
#include <TimeLib.h>
#include <stdint.h>
#include <LCD03.h>

#include "MOVE_RD02.h"
#include "TMR2.h"

#define RD02_I2C_ADDRESS byte((0xB0) >> 1)
#define REFRESH_LIMIT 10000

MOVE_RD02 motor(RD02_I2C_ADDRESS);

LCD03 lcd;
uint16_t refresh_lcd = 0;

// TODO change vars in the interuption

volatile int32_t m1_ticks_prev;
volatile int32_t m1_ticks_curr;

volatile int32_t m2_ticks_prev;
volatile int32_t m2_ticks_curr;

char lcd_display_mdg[32] =
    { 'M', '1', ':', ' ', '.', ' ', ' ', ' ', 'H', 'H', ':', 'M', 'M', ':', 'S', 'S',
      'M', '2', ':', ' ', '.', ' ', ' ', ' ', 'D', 'D', '/', 'M', 'M', '/', 'Y', 'Y' };

volatile uint16_t inter;


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

    // lcd cant work in interuptions
    // TMR2::init();
    // TMR2::set(1000, updateLCD);
    // TMR2::start();
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
            refreshLCD();
    }
}

void refreshLCD()
{
    refresh_lcd = (refresh_lcd + 1) % REFRESH_LIMIT;
    if (refresh_lcd == 0)
    {
        float speed1 = getSpeed1();
        float speed2 = getSpeed2();

        lcd_display_mdg[3] = getDigitFromDouble(speed1, 0);
        lcd_display_mdg[5] = getDigitFromDouble(speed1, 0);
        lcd_display_mdg[6] = getDigitFromDouble(speed1, -1);

        lcd_display_mdg[8] = '0' + hour() / 10;
        lcd_display_mdg[9] = '0' + hour() % 10;

        lcd_display_mdg[11] = '0' + minute() / 10;
        lcd_display_mdg[12] = '0' + minute() % 10;

        lcd_display_mdg[14] = '0' + second() / 10;
        lcd_display_mdg[15] = '0' + second() % 10;

        lcd_display_mdg[19] = getDigitFromDouble(speed2, 0);
        lcd_display_mdg[21] = getDigitFromDouble(speed2, 0);
        lcd_display_mdg[22] = getDigitFromDouble(speed2, -1);

        lcd_display_mdg[24] = '0' + day() / 10;
        lcd_display_mdg[25] = '0' + day() % 10;

        lcd_display_mdg[27] = '0' + month() / 10;
        lcd_display_mdg[28] = '0' + month() % 10;

        lcd_display_mdg[30] = '0' + (year() / 10) % 10;
        lcd_display_mdg[31] = '0' + year() % 10;

        lcd.print(lcd_display_mdg);
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
    lcd.print(lcd_display_mdg);
}

void command()
{
    if (Serial.available())
    {
        String temp = Serial.readStringUntil('\n');
        const char* str = temp.c_str();
        Serial.print("Accepted string: ");
        Serial.println(str);
        if (strcmp(str, "forward\0") == 0)
        {
            motor.moveForward();
            Serial.println("Forward");
        }
        if (strcmp(str, "backwards\0") == 0)
        {
            motor.moveBackward();
        }
        if (strcmp(str, "left\0") == 0)
        {
            motor.turnLeft();
        }
        if (strcmp(str, "rigth\0") == 0)
        {
            motor.turnRigth();
        }
        if (strcmp(str, "spinleft\0") == 0)
        {
            motor.spinLeft();
        }
        if (strcmp(str, "spinrigth\0") == 0)
        {
            motor.spinRigth();
        }
        if (strcmp(str, "stop\0") == 0)
        {
            motor.stopRD();
        }
        /*if(strcmp(str, "speed1\0") == 0){
          motor.getSpeed1();
        }
        if(strcmp(str, "speed2\0") == 0){
          motor.getSpeed2();
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

        }*/
    }
}

// TODO change to normal one
float getSpeed1()
{
    return m1_ticks_curr - m1_ticks_prev;
}

// TODO change to normal one
float getSpeed2()
{
    return m2_ticks_curr - m2_ticks_prev;
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
