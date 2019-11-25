#include "Joystick.hpp"
#include <Arduino.h>
#include "maths.h"
#include "generic.hpp"

float Joystick::GetX(void) const
{
    float value = normalize11((float)analogRead(m_PinX), 0, ANALOG_MAX);
    return fabs(value) >= m_Deadzone ? value : 0.0f;
}

float Joystick::GetY(void) const
{
    float value = normalize11((float)analogRead(m_PinY), 0, ANALOG_MAX);
    return fabs(value) >= m_Deadzone ? value : 0.0f;
}

Joystick::Joystick(const uint8_t pinX, const uint8_t pinY, const uint8_t buttonPin, const float deadzone) : Button(buttonPin), m_PinX(pinX), m_PinY(pinY), m_Deadzone(deadzone)
{
    pinMode(m_PinX, INPUT);
    pinMode(m_PinY, INPUT);
}
