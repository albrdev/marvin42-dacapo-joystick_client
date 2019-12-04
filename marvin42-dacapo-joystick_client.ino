#include <math.h>
#include <Arduino.h>
#include "crc.h"
#include "packet.h"
#include "custom_packets.h"
#include "HC06.hpp"
#include "Button.hpp"
#include "Joystick.hpp"
#include "Regulator.hpp"
#include "generic.hpp"
#include <SoftwareSerial.h>

#include "config.h" // IP/port, WiFi SSID/password

#define D2 2
#define D3 3
#define D10 10
#define D11 11

const unsigned long delayTime = 500;

//HC06 bluetooth(D10, D11);
SoftwareSerial bluetooth(D10, D11);
Joystick leftJoystick(A0, A1, D2, 0.1f, 0.025f);
Joystick rightJoystick(A2, A3, D3, 0.1f, 0.025f);
Regulator speedRegulator( A4, 31.0f / ANALOG_MAX, 1000.0f / ANALOG_MAX, 0.01f);

struct
{
    vector2data_t direction;
    float power;
} inputdata = { { 0.0f, 0.0f }, 0.0f };

void SendDirectionPacket(void)
{
    packet_direction_t pkt;
    packet_mkdirection(&pkt, &inputdata.direction);

    //bluetooth.Write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
}

void SendMotorPowerPacket(void)
{
    packet_motorpower_t pkt;
    packet_mkmotorpower(&pkt, inputdata.power);

    //bluetooth.Write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
}

void SendMotorStopPacket(void)
{
    packet_header_t pkt;
    packet_mkbasic(&pkt, CPT_MOTORSTOP);

    //bluetooth.Write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
}

void onLeftJoystickButtonPressed(const bool value)
{
    if(value)
    {
        PrintDebug("Joystick(Left): Released");
    }
    /*else
    {
        PrintDebug("Joystick(Left): Pressed");
    }*/

    delay(250);
}

void onRightJoystickButtonPressed(const bool value)
{
    if(value)
    {
        PrintDebug("Joystick(Right): Released");
    }
    /*else
    {
        PrintDebug("Joystick(Right): Pressed");
    }*/

    delay(250);
}

void onLeftJoystickAxisChanged(const float x, const float y)
{
    inputdata.direction.x = -x;
    inputdata.direction.y = -y;

    PrintDebug("Joystick(Left): ");
    PrintDebug("x="); PrintDebug(inputdata.direction.x); PrintDebug(", ");
    PrintDebug("y="); PrintDebug(inputdata.direction.y);
    PrintDebugLine("");

    SendDirectionPacket();
}

void onRightJoystickAxisChanged(const float x, const float y)
{
    PrintDebug("Joystick(Right): ");
    PrintDebug("x="); PrintDebug(x); PrintDebug(", ");
    PrintDebug("y="); PrintDebug(y);
    PrintDebugLine("");
}

void onSpeedRegulated(const float oldValue, const float newValue)
{
    inputdata.power = newValue;
    int dir = sgn(inputdata.power - oldValue);

    PrintDebug("Speed: ");
    PrintDebug(dir > 0 ? "Increased to " : "Decreased to "); PrintDebug(inputdata.power);
    PrintDebugLine("");

    SendMotorPowerPacket();
}

/*void setupBluetooth(void)
{
    Serial.println("Initializing Bluetooth device...");
    bluetooth.Begin(HC06::BR_9600);

    Serial.println("Verifying device");
    while(!bluetooth.Ping())
    {
        Serial.println("Failed");
        delay(500UL);
    }

    delay(500UL);
    Serial.print("Version: ");
    const char* version = bluetooth.GetVersion();
    if(version != nullptr)
    {
        Serial.println(version);
    }
    else
    {
        Serial.println("N/A");
    }

    delay(500UL);
    Serial.println("Setting baud rate");

    while(!bluetooth.SetBaudRate(HC06::BR_9600))
    {
        Serial.println("Failed");
        delay(500UL);
    }

    delay(500UL);
    Serial.println("Setting PIN");
    while(!bluetooth.SetPIN("1357"))
    {
        Serial.println("Failed");
        delay(500UL);
    }

    delay(500UL);
    Serial.println("Setting name");
    while(!bluetooth.SetName("Marvin42"))
    {
        Serial.println("Failed");
        delay(500UL);
    }

    Serial.println("");
}*/

void setup(void)
{
    Serial.begin(9600, SERIAL_8N1);
    Serial.println("Initializing...");
    Serial.flush();
    delay(2500);

    Serial.println("Initializing input device...");
    leftJoystick.SetOnAxisChangedEvent(onLeftJoystickAxisChanged);
    leftJoystick.SetOnStateChangedEvent(onLeftJoystickButtonPressed);
    rightJoystick.SetOnAxisChangedEvent(onRightJoystickAxisChanged);
    leftJoystick.SetOnStateChangedEvent(onRightJoystickButtonPressed);
    speedRegulator.SetOnValueChangedEvent(onSpeedRegulated);

    //setupBluetooth();
    bluetooth.begin(9600);

    Serial.println("Done");
    Serial.flush();
}

void loop(void)
{
    leftJoystick.Poll();
    rightJoystick.Poll();
    speedRegulator.Poll();

    //delay(delayTime);
}
