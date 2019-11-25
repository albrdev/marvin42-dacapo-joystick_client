#include <math.h>
#include <Arduino.h>
#include "crc.h"
#include "packet.h"
#include "custom_packets.h"
#include "Button.hpp"
#include "Joystick.hpp"
#include "generic.hpp"

#include "config.h" // IP/port, WiFi SSID/password

#define D2 2

const unsigned long delayTime = 500;

Joystick joystick(A0, A1, D2, 0.05);

float lastX = 0.0f;
float lastY = 0.0f;

void onJoystickButtonPressed(const bool value)
{
    if(value)
    {
        //PrintDebugLine("Joystick button released");
        PrintDebugLine("Joystick button");
    }
    /*else
    {
        //PrintDebugLine("Joystick button pressed");
    }*/

    delay(250);
}

void setup(void)
{
    Serial.begin(9600);
    Serial.println("Initializing...");
    Serial.flush();
    delay(2500);

    joystick.SetOnStateChangedEvent(onJoystickButtonPressed);

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
    joystick.Poll();
    float x = joystick.GetX();
    float y = joystick.GetY();
    //PrintDebug("Input: "); PrintDebug("x="); PrintDebug(x); PrintDebug(", y="); PrintDebugLine(y);

    if(absdiff(x, lastX) <= 0.05f && absdiff(y, lastY) <= 0.05f)
    {
        return;
    }

    //PrintDebug("Sending: "); PrintDebug("x="); PrintDebug(x); PrintDebug(", y="); PrintDebugLine(y);
    SendMotorRunPacket(x, y);

    lastX = x;
    lastY = y;

    //delay(delayTime);
}
