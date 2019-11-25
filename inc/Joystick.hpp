#ifndef _JOYSTICK_HPP_
#define _JOYSTICK_HPP_

#include <stdint.h>

class Joystick {
    public:
    Joystick(uint8_t pinX, uin8_t pinY, float deadzone = 0.05f);
    float GetX();
    float GetY();

    private:
    float m_Deadzone;
    uint8_t m_PinX;
    uint8_t m_PinY;

};

#endif
