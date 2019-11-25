#include <math.h>
#include <Arduino.h>
#include "crc.h"
#include "packet.h"
#include "custom_packets.h"
#include "Button.hpp"
#include "Joystick.hpp"
#include "Regulator.hpp"
#include "generic.hpp"

#include "config.h" // IP/port, WiFi SSID/password

#define D2 2
#define D3 3

const unsigned long delayTime = 500;

Joystick leftJoystick(A0, A1, D2, 0.05);
Joystick rightJoystick(A2, A3, D3, 0.05);
Regulator speedRegulator(A4, 22.0f / ANALOG_MAX, 1000.0f / ANALOG_MAX, 0.005);

float lastX = 0.0f;
float lastY = 0.0f;

void onLeftJoystickButtonPressed(const bool value)
{
    if(value)
    {
        //PrintDebugLine("Joystick button released");
        PrintDebugLine("Left joystick button");
    }
    /*else
    {
        //PrintDebugLine("Joystick button pressed");
    }*/

    delay(250);
}

void onRightJoystickButtonPressed(const bool value)
{
    if(value)
    {
        //PrintDebugLine("Joystick button released");
        PrintDebugLine("Right joystick button");
    }
    /*else
    {
        //PrintDebugLine("Joystick button pressed");
    }*/

    delay(250);
}

void onSpeedRegulated(const float oldValue, const float newValue)
{
    int dir = sgn(newValue - oldValue);
    PrintDebug("Speed: "); PrintDebug(dir > 0 ? "Increased to " : "Decreased to "); PrintDebugLine(newValue);
}

void setup(void)
{
    Serial.begin(9600);
    Serial.println("Initializing...");
    Serial.flush();
    delay(2500);

    leftJoystick.SetOnStateChangedEvent(onLeftJoystickButtonPressed);
    leftJoystick.SetOnStateChangedEvent(onRightJoystickButtonPressed);
    speedRegulator.SetOnValueChangedEvent(onSpeedRegulated);

    Serial.println("Done");
    Serial.flush();
}

void SendMotorRunPacket(const float x, const float y)
{
    packet_motorrun_t pkt;
    packet_mkmotorrun(&pkt, x, y);

    // TODO: Send
}

void SendMotorStopPacket(void)
{
    packet_header_t pkt;
    packet_mkbasic(&pkt, CPT_MOTORSTOP);

    // TODO: Send
}

void loop(void)
{
    leftJoystick.Poll();
    rightJoystick.Poll();
    speedRegulator.Poll();

    float x1 = leftJoystick.GetX();
    float y1 = leftJoystick.GetY();
    float x2 = rightJoystick.GetX();
    float y2 = rightJoystick.GetY();
    //PrintDebug("Joystick(1): "); PrintDebug("x="); PrintDebug(x1); PrintDebug(", y="); PrintDebugLine(y1);
    //PrintDebug("Joystick(2): "); PrintDebug("x="); PrintDebug(x2); PrintDebug(", y="); PrintDebugLine(y2);

    if(absdiff(x1, lastX) <= 0.05f && absdiff(y1, lastY) <= 0.05f)
    {
        return;
    }

    //PrintDebug("Sending: "); PrintDebug("x="); PrintDebug(x); PrintDebug(", y="); PrintDebugLine(y);
    SendMotorRunPacket(x1, y1);

    lastX = x1;
    lastY = y1;

    //delay(delayTime);
}
