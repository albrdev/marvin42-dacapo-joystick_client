#ifndef __REGULATOR_HPP__
#define __REGULATOR_HPP__

#include <stdint.h>

class Regulator
{
public:
    typedef void(*OnValueChangedEventHandler)(const float, const float);

private:
    uint8_t m_Pin;

    float m_LowerCap;
    float m_UpperCap;
    float m_Tolerance;

    float m_Value = 0.0f;

    OnValueChangedEventHandler m_OnValueChangedEvent = nullptr;

public:
    float GetValue(void) const;

    void SetOnValueChangedEvent(const OnValueChangedEventHandler value);

    virtual void Poll(void);

    Regulator(const uint8_t pin, const float lowerCap = 0.0f, const float upperCap = 1.0f, const float tolerance = 0.0f);
};

#endif // __REGULATOR_HPP__
