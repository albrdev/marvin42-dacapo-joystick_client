#ifndef __JOYSTICK_HPP__
#define __JOYSTICK_HPP__

#include <stdint.h>
#include "Button.hpp"

class Joystick : public Button
{
public:
    typedef void(*OnAxisChangedEventHandler)(const float, const float);

    Joystick(const uint8_t pinX, const uint8_t pinY, const uint8_t buttonPin, const float deadzone = 0.05f, const float tolerance = 0.0f);

    virtual void Poll(void) override;

    void SetOnAxisChangedEvent(const OnAxisChangedEventHandler value);

    float GetX(void) const;
    float GetY(void) const;

private:
    uint8_t m_PinX;
    uint8_t m_PinY;

    float m_Deadzone;
    float m_Tolerance;

    OnAxisChangedEventHandler m_OnAxisChangedEvent = nullptr;

    float m_X = 0.0f;
    float m_Y = 0.0f;
};

#endif // __JOYSTICK_HPP__
