#include "InterruptIn.hpp"

class RotaryEncoder : public InterruptIn
{
public:
    typedef void(*OnValueChangedEventHandler)(const int value, const bool direction);
    static const unsigned int MaxCount = 40;

private:
    uint8_t m_CLKPin;
    uint8_t m_DTPin;

    uint8_t m_LastCLKState;
    int m_Counter = 0;

    OnValueChangedEventHandler m_OnValueChangedEvent = nullptr;

public:
    void SetOnValueChangedCallback(const OnValueChangedEventHandler value);

    virtual void Poll(void) override;

    RotaryEncoder(const uint8_t clkPin, const uint8_t dtPin, const uint8_t swPin);
};
