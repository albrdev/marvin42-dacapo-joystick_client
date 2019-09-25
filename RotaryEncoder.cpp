#include "RotaryEncoder.hpp"
#include <Arduino.h>

void RotaryEncoder::SetOnValueChangedCallback(const OnValueChangedEventHandler value)
{
    m_OnValueChangedEvent = value;
}

void RotaryEncoder::Poll(void)
{
    InterruptIn::Poll();

    uint8_t clkState = digitalRead(m_CLKPin);
    uint8_t dtState = digitalRead(m_DTPin);
    if(clkState != m_LastCLKState)
    {
        bool cw = dtState == clkState;

        m_Counter += cw ? 1 : -1;
        m_LastCLKState = clkState;

        if(m_Counter > 0 && m_Counter % MaxCount == 0)
        {
            m_Counter = 0;
        }

        if(m_OnValueChangedEvent != nullptr)
            m_OnValueChangedEvent(m_Counter, cw);
    }
}

RotaryEncoder::RotaryEncoder(const uint8_t clkPin, const uint8_t dtPin, const uint8_t swPin) : InterruptIn(swPin)
{
    m_CLKPin = clkPin;
    m_DTPin = dtPin;

    pinMode(m_CLKPin, INPUT);
    pinMode(m_DTPin, INPUT);

    m_LastCLKState = digitalRead(m_CLKPin);
}
