#include "InterruptIn.hpp"
#include <Arduino.h>    /* pinMode(), digitalRead() */

bool InterruptIn::GetState(void) const
{
    return digitalRead(m_Pin);
}

void InterruptIn::SetOnStateChangedEvent(const OnStateChangedEventHandler value)
{
    m_OnStateChangedEvent = value;
}

void InterruptIn::Poll(void)
{
    bool state = digitalRead(m_Pin);
    if(state != m_State)
    {
        m_State = state;
        if(m_OnStateChangedEvent != nullptr)
            m_OnStateChangedEvent(m_State);
    }
}

InterruptIn::InterruptIn(const uint8_t pin) : m_Pin(pin)
{
    pinMode(m_Pin, INPUT);
    m_State = digitalRead(m_Pin);
}
