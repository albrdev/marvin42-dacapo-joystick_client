#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include <stdint.h>

class Button
{
public:
    /*! OnStateChangedEventHandler().
    \typedef OnStateChangedEventHandler().
    \param Current state of the button
    \return N/A
    */
    typedef void(*OnStateChangedEventHandler)(const bool);

private:
    uint8_t m_Pin; /*!< The pin number */
    bool m_State; /*!< Holds the last read state of the button */
        
    OnStateChangedEventHandler m_OnStateChangedEvent = nullptr; /*!< Pointer to callback function to fire when the button's state has changed */

public:
    /*! GetState().
    \fn GetState().
    \param N/A
    \return Current state of the button
    */
    bool GetState(void) const;
    
    /*! SetOnStateChangedEvent().
    \fn SetOnStateChangedEvent().
    \param Pointer to a callback function
    \return N/A
    */
    void SetOnStateChangedEvent(const OnStateChangedEventHandler value);

    /*! Poll().
    \fn Poll().
    \param N/A
    \return N/A
    */
    virtual void Poll(void);

    /*! Button().
    \fn Button().
    \param The pin number to read from
    \return N/A
    */
    Button(const uint8_t pin);
};

#endif // __BUTTON_HPP__
