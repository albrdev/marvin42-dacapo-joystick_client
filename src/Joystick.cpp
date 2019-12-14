#include "Joystick.hpp"
#include <Arduino.h>
#include "maths.h"
#include "generic.hpp"

float Joystick::GetX(void) const
{
    float value = normalize11((float)analogRead(m_PinX), 0, ANALOG_MAX); // Normalize value to be between -1 and 1
    return fabs(value) >= m_Deadzone ? value : 0.0f; // Only return the actual value when it has reached the deadzone (This can prevent extra sensitive readings when the joystick is barely touched)
}

float Joystick::GetY(void) const
{
    float value = normalize11((float)analogRead(m_PinY), 0, ANALOG_MAX); // Normalize value to be between -1 and 1
    return fabs(value) >= m_Deadzone ? value : 0.0f; // Only return the actual value when it has reached the deadzone (This can prevent extra sensitive readings when the joystick is barely touched)
}

void Joystick::SetOnAxisChangedEvent(const OnAxisChangedEventHandler value)
{
    m_OnAxisChangedEvent = value;
}

void Joystick::Poll(void)
{
    float x = GetX();
    float y = GetY();
    // Only tolerate change when read values differ too much from the previous ones
    if(absdiff(x, m_X) >= m_Tolerance || absdiff(y, m_Y) >= m_Tolerance)
    {
        if(m_OnAxisChangedEvent != nullptr)
        {
            m_OnAxisChangedEvent(x, y);
        }

        m_X = x;
        m_Y = y;
    }

    Button::Poll(); // Poll the joystick button
}

Joystick::Joystick(const uint8_t pinX, const uint8_t pinY, const uint8_t buttonPin, const float deadzone, const float tolerance) : Button(buttonPin), m_PinX(pinX), m_PinY(pinY), m_Deadzone(deadzone), m_Tolerance(tolerance)
{
    pinMode(m_PinX, INPUT);
    pinMode(m_PinY, INPUT);
}
