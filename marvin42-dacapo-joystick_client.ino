#include <math.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "crc.h"
#include "packet.h"
#include "custom_packets.h"
#include "StreamCommandHandler.hpp"
#include "Button.hpp"
#include "Joystick.hpp"
#include "Regulator.hpp"
#include "generic.hpp"

#define JOYSTICK_LEFT_BUTTON 2
#define JOYSTICK_RIGHT_BUTTON 3
#define SERIAL_RX 11
#define SERIAL_TX 12

const unsigned long delayTime = 500;

//#define WIRED_COM

#ifndef WIRED_COM
SoftwareSerial bluetooth(SERIAL_RX, SERIAL_TX);
char commandBuffer[27U + 1U];
StreamCommandHandler commandHandler(bluetooth, commandBuffer);
#else
//#define bluetooth Serial
SoftwareSerial bluetooth(SERIAL_RX, SERIAL_TX);
#endif
Joystick leftJoystick(A0, A1, JOYSTICK_LEFT_BUTTON, 0.1f, 0.025f);
Joystick rightJoystick(A2, A3, JOYSTICK_RIGHT_BUTTON, 0.1f, 0.025f);
Regulator speedRegulator(A4, 0.1f, 0.9f, 0.1f);

struct
{
    struct
    {
        vector2data_t direction;
        float power;
    } movement;

    struct
    {
        int8_t direction;
        float power;
    } rotation;
} inputdata = { { { 0.0f, 0.0f }, 0.0f}, { 0, 0.0f } };

#define KA_INTERVAL (1UL * 1000UL)
unsigned long int nextKeepAlive = 0UL;

void SendDirectionPacket(void)
{
    packet_direction_t pkt;
    packet_mkdirection(&pkt, &inputdata.movement.direction);

    #ifndef WIRED_COM
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #else
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #endif

    nextKeepAlive = millis() + KA_INTERVAL;
}

void SendMotorPowerPacket(void)
{
    packet_motorpower_t pkt;
    packet_mkmotorpower(&pkt, inputdata.movement.power);

    #ifndef WIRED_COM
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #else
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #endif

    nextKeepAlive = millis() + KA_INTERVAL;
}

void SendMotorRotationPacket(void)
{
    packet_motorrotation_t pkt;
    packet_mkmotorrotation(&pkt, inputdata.rotation.direction, inputdata.rotation.power);

    #ifndef WIRED_COM
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #else
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #endif

    nextKeepAlive = millis() + KA_INTERVAL;
}

void SendMotorStopPacket(void)
{
    packet_header_t pkt;
    packet_mkbasic(&pkt, CPT_MOTORSTOP);

    #ifndef WIRED_COM
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #else
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #endif

    nextKeepAlive = millis() + KA_INTERVAL;
}

void SendKeepAlivePacket(void)
{
    packet_header_t pkt;
    packet_mkbasic(&pkt, PT_SYN);

    #ifndef WIRED_COM
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #else
    bluetooth.write((const uint8_t*)&pkt, sizeof(pkt));
    bluetooth.flush();
    #endif

    nextKeepAlive = millis() + KA_INTERVAL;
}

void onLeftJoystickButtonPressed(const bool value)
{
    PrintDebug("Joystick(Left): "); PrintDebug(!value ? "Pressed" : "Released");
    PrintDebugLine();

    if(!value)
    {
        SendMotorStopPacket();
    }

    delay(250);
}

void onLeftJoystickAxisChanged(const float x, const float y)
{
    inputdata.movement.direction.x = -x;
    inputdata.movement.direction.y = -y;

    SendDirectionPacket();

    PrintDebug("Joystick(Left): ");
    PrintDebug("x="); PrintDebug(inputdata.movement.direction.x); PrintDebug(", ");
    PrintDebug("y="); PrintDebug(inputdata.movement.direction.y);
    PrintDebugLine();
}

void onLeftJoystickAxisChanged2(float x, float y)
{
    x = clamp11((float)roundf(-x));
    y = clamp11((float)roundf(-y));

    if(x == inputdata.movement.direction.x && y == inputdata.movement.direction.y)
        return;

    inputdata.movement.direction.x = x;
    inputdata.movement.direction.y = y;

    SendDirectionPacket();

    PrintDebug("Joystick(Left): ");
    PrintDebug("x="); PrintDebug(inputdata.movement.direction.x); PrintDebug(", ");
    PrintDebug("y="); PrintDebug(inputdata.movement.direction.y);
    PrintDebugLine();
}

void onRightJoystickButtonPressed(const bool value)
{
    PrintDebug("Joystick(Right): "); PrintDebug(!value ? "Pressed" : "Released");
    PrintDebugLine();

    if(!value)
    {
        SendMotorStopPacket();
    }

    delay(250);
}

void onRightJoystickAxisChanged(float x, float y)
{
    if(x == 0.0f)
    {
        if(inputdata.rotation.direction != 0)
        {
            inputdata.rotation.direction = 0;
            SendMotorStopPacket();
        }

        return;
    }

    x = clamp11((float)roundf(-x));
    //y = clamp11((float)roundf(-y));

    if(x == inputdata.rotation.direction)
        return;

    inputdata.rotation.direction = x;
    inputdata.rotation.power = 1.0f;

    SendMotorRotationPacket();

    PrintDebug("Joystick(Right): ");
    PrintDebug("x="); PrintDebug(x); PrintDebug(", ");
    PrintDebug("y="); PrintDebug(y);
    PrintDebugLine();
}

void onSpeedRegulated(const float oldValue, const float newValue)
{
    inputdata.movement.power = newValue;

    SendMotorPowerPacket();

    PrintDebug("Speed: ");
    PrintDebug((newValue - oldValue) > 0.0f ? "Increased to " : "Decreased to "); PrintDebug(inputdata.movement.power);
    PrintDebugLine();
}

void setupBluetooth(void)
{
    #ifndef WIRED_COM
    Serial.println("Initializing Bluetooth device...");
    bluetooth.begin(115200);

    return;

    Serial.println("Verifying...");
    while(!commandHandler.PrintCommand("AT", "OK"))
    {
        Serial.println("Failed");
        delay(500UL);
    }
    delay(500UL);

    #define BT_VER "linvorV1.8"
    Serial.print("Version: ");
    commandHandler.Print("AT+VERSION");
    const char* version = commandHandler.GetResponse(12U);
    Serial.println(version != nullptr ? version + 2U : "N/A");
    delay(500UL);

    Serial.println("Setting name...");
    while(!commandHandler.PrintCommand("AT+NAMEMarvin42-Joystick", "OKsetname"))
    {
        Serial.println("Failed");
        delay(500UL);
    }
    delay(500UL);

    Serial.println("Setting PIN...");
    while(!commandHandler.PrintCommand("AT+PIN1357", "OKsetPIN"))
    {
        Serial.println("Failed");
        delay(500UL);
    }
    delay(500UL);

    Serial.println("Setting baud rate...");
    while(!commandHandler.PrintCommand("AT+BAUD4", "OK9600"))
    {
        Serial.println("Failed");
        delay(500UL);
    }
    delay(500UL);

    bluetooth.begin(9600);

    Serial.println();
    #else
    bluetooth.begin(115200);
    bluetooth.print("$");
    bluetooth.print("$");
    bluetooth.print("$");
    delay(100);
    /*bluetooth.println("SU,115200,N");
    delay(100);
    bluetooth.println("SR,B827EB3315BD");
    delay(100);
    bluetooth.println("SM,3");
    delay(100);
    bluetooth.println("SP,1357");
    delay(100);*/
    bluetooth.println("---");
    bluetooth.flush();
    #endif
}

void setup(void)
{
    delay(2500);
    Serial.begin(9600, SERIAL_8N1);
    Serial.println("Initializing...");

    Serial.println("Initializing input device...");
    leftJoystick.SetOnAxisChangedEvent(onLeftJoystickAxisChanged2);
    leftJoystick.SetOnStateChangedEvent(onLeftJoystickButtonPressed);
    rightJoystick.SetOnAxisChangedEvent(onRightJoystickAxisChanged);
    leftJoystick.SetOnStateChangedEvent(onRightJoystickButtonPressed);
    speedRegulator.SetOnValueChangedEvent(onSpeedRegulated);

    setupBluetooth();

    Serial.println("Done");
    Serial.println();
    Serial.flush();
}

void loop(void)
{
    leftJoystick.Poll();
    rightJoystick.Poll();
    speedRegulator.Poll();

    if((long)(millis() - nextKeepAlive) >= 0L)
    {
        SendKeepAlivePacket();
    }

    //delay(delayTime);
}
