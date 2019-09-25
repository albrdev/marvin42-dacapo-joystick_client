#ifndef __INTERRUPTIN_HPP__
#define __INTERRUPTIN_HPP__

#include <stdint.h> /* uint8_t */

typedef void(*interruptin_triggercallback_t)(const bool);

class InterruptIn
{
private:
    uint8_t m_Pin;
    bool m_State;

    interruptin_triggercallback_t m_OnStateChangedCallback = nullptr;

public:
    bool GetState(void) const;
    void SetOnStateChangedCallback(const interruptin_triggercallback_t value);

    virtual void Poll(void);

    InterruptIn(const uint8_t pin);
};

#endif // __INTERRUPTIN_HPP__
