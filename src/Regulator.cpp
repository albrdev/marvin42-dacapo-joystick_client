#include "Regulator.hpp"
#include <Arduino.h>
#include "maths.h"
#include "generic.hpp"

float Regulator::GetValue(void) const
{
    float value = normalize01((float)analogRead(m_Pin), 0, ANALOG_MAX); // Nomralize the absolute min/max values we could read
    value = clamp(value, m_LowerCap, m_UpperCap);                       // Clamp them to the custom min/max caps
    return normalize01(value, m_LowerCap, m_UpperCap);                  // Normalize again to get a 0-1 range with our custom value caps
}

void Regulator::SetOnValueChangedEvent(const OnValueChangedEventHandler value)
{
    m_OnValueChangedEvent = value;
}

void Regulator::Poll(void)
{
    float value = GetValue();
    // Only tolerate change when read value differs too much from the previous one
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
