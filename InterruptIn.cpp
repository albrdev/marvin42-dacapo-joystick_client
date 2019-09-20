#include "InterruptIn.hpp"
#include <Arduino.h>    /* pinMode(), digitalRead() */

bool InterruptIn::GetState(void) const
{
    return digitalRead(m_Pin);
}

void InterruptIn::SetOnStateChangedCallback(const interruptin_triggercallback_t value)
{
    m_OnStateChangedCallback = value;
}

void InterruptIn::Poll(void)
{
    bool state = digitalRead(m_Pin);
    if(state != m_State)
    {
        m_State = state;
        if(m_OnStateChangedCallback != nullptr) { m_OnStateChangedCallback(m_State); }
    }
}

InterruptIn::InterruptIn(const uint8_t pin)
{
    m_Pin = pin;

    pinMode(m_Pin, INPUT);
    m_State = digitalRead(m_Pin);
}
