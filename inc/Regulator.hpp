#ifndef __REGULATOR_HPP__
#define __REGULATOR_HPP__

#include <stdint.h>

class Regulator
{
public:
    /*! OnValueChangedEventHandler().
    \fn OnValueChangedEventHandler().
    \param .
    \return fabs.
    */
    typedef void(*OnValueChangedEventHandler)(const float, const float);

private:
    uint8_t m_Pin; /*!< TODO */

    float m_LowerCap; /*!< TODO */
    float m_UpperCap; /*!< TODO */
    float m_Tolerance; /*!< TODO */

    float m_Value = 0.0f; /*!< TODO */

    OnValueChangedEventHandler m_OnValueChangedEvent = nullptr; /*!< TODO */

public:
    /*! GetValue().
    \fn GetValue().
    \param .
    \return .
    */
    float GetValue(void) const;

    /*! SetOnValueChangedEvent().
    \fn SetOnValueChangedEvent().
    \param .
    \return .
    */
    void SetOnValueChangedEvent(const OnValueChangedEventHandler value);

    /*! Poll().
    \fn Poll().
    \param .
    \return .
    */
    virtual void Poll(void);

    Regulator(const uint8_t pin, const float lowerCap = 0.0f, const float upperCap = 1.0f, const float tolerance = 0.0f);
};

#endif // __REGULATOR_HPP__
