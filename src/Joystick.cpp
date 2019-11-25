#include "Joystick.hpp"
#include <Arduino.h>
#include "maths.h"

float Joystick::GetX(){
    float value = normalize11((float)analogRead(m_PinX), 0, 1023);
    return fabs(value) >= m_Deadzone ? value : 0.0f;
}

float Joystick::GetY(){
    float value = normalize11((float)analogRead(m_PinY), 0, 1023);
    return fabs(value) >= m_Deadzone ? value : 0.0f;
}

Joystick::Joystick(uint8_t pinX, uin8_t pinY, float deadzone)
{
    m_PinX = pinX;
    m_PinY = pinY;

    m_Deadzone = deadzone;

    pinMode(m_PinX, INPUT);
    pinMode(m_PinY, INPUT);
}
