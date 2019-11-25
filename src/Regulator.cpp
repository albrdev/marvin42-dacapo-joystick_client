#include "Regulator.hpp"
#include <Arduino.h>
#include "maths.h"
#include "generic.hpp"

float Regulator::GetValue(void) const
{
    float value = normalize01((float)analogRead(m_Pin), 0, ANALOG_MAX);
    return value < m_LowerCap ? 0.0f : (value > m_UpperCap ? 1.0f : value);
}

void Regulator::SetOnValueChangedEvent(const OnValueChangedEventHandler value)
{
    m_OnValueChangedEvent = value;
}

void Regulator::Poll(void)
{
    float value = GetValue();
    if(absdiff(value, m_Value) >= m_Tolerance)
    {
        if(m_OnValueChangedEvent != nullptr)
        {
            m_OnValueChangedEvent(m_Value, value);
        }

        m_Value = value;
    }
}

Regulator::Regulator(const uint8_t pin, const float lowerCap, const float upperCap, const float tolerance) : m_Pin(pin), m_LowerCap(lowerCap), m_UpperCap(upperCap), m_Tolerance(tolerance)
{
    pinMode(pin, INPUT);
}
