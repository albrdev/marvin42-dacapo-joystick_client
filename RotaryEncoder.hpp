#include "InterruptIn.hpp"

class RotaryEncoder : public InterruptIn
{
public:
    typedef void(*OnValueChangedEventHandler)(const int value, const bool direction);
    typedef void(*OnRevolutionEventHandler)(const bool direction);
    static const int MaxCount = 40;

private:
    uint8_t m_CLKPin = 0;
    uint8_t m_DTPin = 0;

    uint8_t m_LastCLKState = 0;
    int m_Counter = 0;

    OnValueChangedEventHandler m_OnValueChangedEvent = nullptr;
    OnRevolutionEventHandler m_OnRevolutionEvent = nullptr;

public:
    void SetOnValueChangedEvent(const OnValueChangedEventHandler value);
    void SetOnRevolutionEvent(const OnRevolutionEventHandler value);

    virtual void Poll(void) override;

    RotaryEncoder(const uint8_t clkPin, const uint8_t dtPin, const uint8_t swPin);
};
