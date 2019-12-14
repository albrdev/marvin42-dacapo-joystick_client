#ifndef __JOYSTICK_HPP__
#define __JOYSTICK_HPP__

#include <stdint.h>
#include "Button.hpp"

class Joystick : public Button
{
public:
    /*! OnAxisChangedEventHandler().
    \fn OnAxisChangedEventHandler().
    \param .
    \return fabs.
    */
    typedef void(*OnAxisChangedEventHandler)(const float, const float);

private:
    uint8_t m_PinX; /*!< map PinX to designated hardware pin */
    uint8_t m_PinY; /*!< map PinX to designated hardware pin */

    float m_Deadzone; /*!< setting up the deadzone where joystick should not react */
    float m_Sensitivity; /*!< setting up the sensitivity */

    OnAxisChangedEventHandler m_OnAxisChangedEvent = nullptr;

    float m_X = 0.0f; /*!< different step up/down on x-axis */
    float m_Y = 0.0f; /*!< different step up/down on y-axis */

public:
    /*! GetX() coordinats.
    \fn GetX().
    \param .
    \return fabs.
    */
    float GetX(void) const;

    /*! GetY() coordinats.
    \fn GetY().
    \param .
    \return fabs.
    */
    float GetY(void) const;

    /*! SetOnAxisChangedEvent().
    \fn SetOnAxisChangedEvent().
    \param .
    \return fabs.
    */
    void SetOnAxisChangedEvent(const OnAxisChangedEventHandler value);

    /*! Poll().
    \fn Poll().
    \param .
    \return fabs.
    */
    virtual void Poll(void) override;

    Joystick(const uint8_t pinX, const uint8_t pinY, const uint8_t buttonPin, const float deadzone = 0.05f, const float sensitivity = 0.0f);
};

#endif // __JOYSTICK_HPP__
