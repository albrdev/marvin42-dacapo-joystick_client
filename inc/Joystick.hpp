#ifndef _JOYSTICK_HPP_
#define _JOYSTICK_HPP_

#include <stdint.h>
#include "Button.hpp"

class Joystick : public Button
{
    public:
    Joystick(const uint8_t pinX, const uint8_t pinY, const uint8_t buttonPin, const float deadzone = 0.05f);
    float GetX(void) const;
    float GetY(void) const;

    private:
    uint8_t m_PinX;
    uint8_t m_PinY;

    float m_Deadzone;
};

#endif