#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include <stdint.h> /* uint8_t */

class Button
{
public:
    /*! OnStateChangedEventHandler().
    \fn OnStateChangedEventHandler().
    \param .
    \return .
    */
    typedef void(*OnStateChangedEventHandler)(const bool);

private:
    uint8_t m_Pin; /*!< map Pin to designated hardware pin */
    bool m_State; /*!< setting the state */

    OnStateChangedEventHandler m_OnStateChangedEvent = nullptr;

public:
    /*! GetState().
    \fn GetState().
    \param .
    \return .
    */
    bool GetState(void) const;
    
    /*! SetOnStateChangedEvent().
    \fn SetOnStateChangedEvent().
    \param .
    \return .
    */
    void SetOnStateChangedEvent(const OnStateChangedEventHandler value);

    /*! Poll().
    \fn Poll().
    \param .
    \return .
    */
    virtual void Poll(void);

    /*! Button().
    \fn Button().
    \param .
    \return .
    */
    Button(const uint8_t pin);
};

#endif // __BUTTON_HPP__
