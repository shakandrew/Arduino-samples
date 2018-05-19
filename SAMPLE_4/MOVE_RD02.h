/*
 * Now it is only .h file with class inside
 * Every time there are some changes, and I can't just set it like a library
*/
#ifndef MOVE_RD02_H
#define MOVE_RD02_H

#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

// Defaults block

#define SPEED1_REGISTER byte(0) // mode 0,1
#define SPEED2_REGISTER byte(1) // mode 0,1
#define SPEED_REGISTER byte(0) // mode 2,3
#define TURN_REGISTER byte(1) // mode 2,3

#define ENC1A_REGISTER byte(2)
#define ENC1B_REGISTER byte(3)
#define ENC1C_REGISTER byte(4)
#define ENC1D_REGISTER byte(5)

#define ENC2A_REGISTER byte(6)
#define ENC2B_REGISTER byte(7)
#define ENC2C_REGISTER byte(8)
#define ENC2D_REGISTER byte(9)

#define MODE_REGISTER byte(15)
#define MODE0 byte(0)
#define MODE1 byte(1)
#define MODE2 byte(2)
#define MODE3 byte(3)

#define DEFAULT_FORWARD_SREED int8_t(64)
#define DEFAULT_BACKWARD_SREED int8_t(-64)

#define DEFAULT_RTURN_SPEED int8_t(-32)
#define DEFAULT_LTURN_SPEED int8_t(32)

#define DEFAULT_RSPIN_SPEED int8_t(64)
#define DEFAULT_LSPIN_SPEED int8_t(-64)
// I would like to work in 1 and 3 modes
// Not with byte, but with char vars

// end

class MOVE_RD02
{
public:
    int8_t address;
    MOVE_RD02(int8_t);
    ~MOVE_RD02();

    void moveForward();
    void moveForward(int8_t);

    void moveBackward();
    void moveBackward(int8_t);

    void turnRigth();
    void turnLeft();
    void turn(int8_t, int8_t);

    void spinRigth();
    void spinLeft();
    void spin(int8_t);

    void stopRD();

    void setSpeed1(int8_t);
    void setSpeed2(int8_t);

    int8_t getSpeed1();
    int8_t getSpeed2();

    int32_t getENC1();
    int32_t getENC2();

    void writeRegister(int8_t, int8_t);
    int8_t readRegister(int8_t);

    int64_t readRegister(int8_t, int8_t);
    void changeMode();
};

MOVE_RD02::MOVE_RD02(int8_t address)
    : address(address)
{
}

MOVE_RD02::~MOVE_RD02()
{
}

// Move Forward
void MOVE_RD02::moveForward()
{
    moveForward(DEFAULT_FORWARD_SREED);
}
void MOVE_RD02::moveForward(int8_t speed)
{
    writeRegister(MODE_REGISTER, MODE3);
    writeRegister(SPEED1_REGISTER, speed);
    writeRegister(SPEED2_REGISTER, 0);
}

// Move Backward
void MOVE_RD02::moveBackward()
{
    moveBackward(DEFAULT_BACKWARD_SREED);
}
void MOVE_RD02::moveBackward(int8_t speed)
{
    writeRegister(MODE_REGISTER, MODE3);
    writeRegister(SPEED1_REGISTER, speed);
    writeRegister(SPEED2_REGISTER, 0);
}

// Turn
void MOVE_RD02::turnRigth()
{
    turn(DEFAULT_FORWARD_SREED, DEFAULT_RTURN_SPEED);
}
void MOVE_RD02::turnLeft()
{
    turn(DEFAULT_FORWARD_SREED, DEFAULT_LTURN_SPEED);
}
void MOVE_RD02::turn(int8_t speed, int8_t turn)
{
    writeRegister(MODE_REGISTER, MODE3);
    writeRegister(SPEED1_REGISTER, speed);
    writeRegister(SPEED2_REGISTER, turn);
}

// Spin
void MOVE_RD02::spinLeft()
{
    spin(DEFAULT_LSPIN_SPEED);
}
void MOVE_RD02::spinRigth()
{
    spin(DEFAULT_RSPIN_SPEED);
}
void MOVE_RD02::spin(int8_t speed)
{
    writeRegister(MODE_REGISTER, MODE3);
    writeRegister(SPEED1_REGISTER, 0);
    writeRegister(SPEED2_REGISTER, speed);
}

// Stop
void MOVE_RD02::stopRD()
{
    writeRegister(MODE_REGISTER, MODE3);
    writeRegister(SPEED1_REGISTER, 0);
    writeRegister(SPEED2_REGISTER, 0);
}

void MOVE_RD02::setSpeed1(int8_t speed)
{ /**/
}

void MOVE_RD02::setSpeed2(int8_t speed)
{ /**/
}

int8_t MOVE_RD02::getSpeed1()
{
    Wire.beginTransmission(address);
    Wire.write(SPEED1_REGISTER);
    Wire.endTransmission();

    Wire.requestFrom(address, byte(1));
    while (!Wire.available())
        ;
    return Wire.read();
}

int8_t MOVE_RD02::getSpeed2()
{
    Wire.beginTransmission(address);
    Wire.write(SPEED2_REGISTER);
    Wire.endTransmission();

    Wire.requestFrom(address, byte(1));
    while (!Wire.available())
        ;
    return Wire.read();
}

int32_t MOVE_RD02::getENC1()
{
    return (int32_t)readRegister(ENC1A_REGISTER, 4);
}

int32_t MOVE_RD02::getENC2()
{
    return (int32_t)readRegister(ENC2A_REGISTER, 4);
}

void MOVE_RD02::writeRegister(int8_t reg, int8_t value)
{
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

int64_t MOVE_RD02::readRegister(int8_t start_reg, int8_t bytes)
{
    int64_t result;
    Wire.beginTransmission(address);
    Wire.write(start_reg);
    Wire.endTransmission();

    Wire.requestFrom(address, bytes);
    while (Wire.available() < bytes)
        ;

    for (int i = 0; i < bytes; i++)
    {
        result |= Wire.read();
        if (i == (bytes - 1))
            break;
        result <<= 8;
    }
    return result;
}

#endif // MOVE_RD02_H
