#include <math.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
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

SoftwareSerial transmitter(SERIAL_RX, SERIAL_TX);

#define COM_BLUETOOTH
#ifdef COM_BLUETOOTH
char commandBuffer[27U + 1U];
StreamCommandHandler commandHandler(transmitter, commandBuffer);
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

#define KA_INTERVAL 1000UL
unsigned long int nextKeepAlive = 0UL;

void Send(const void* const data, const size_t size)
{
    transmitter.write((const uint8_t*)data, size);
    transmitter.flush();

    nextKeepAlive = millis() + KA_INTERVAL;
}

void SendDirectionPacket(void)
{
    packet_direction_t pkt;
    packet_mkdirection(&pkt, &inputdata.movement.direction);

    Send(&pkt, sizeof(pkt));
}

void SendMotorPowerPacket(void)
{
    packet_motorpower_t pkt;
    packet_mkmotorpower(&pkt, inputdata.movement.power);

    Send(&pkt, sizeof(pkt));
}

void SendMotorRotationPacket(void)
{
    packet_motorrotation_t pkt;
    packet_mkmotorrotation(&pkt, inputdata.rotation.direction, inputdata.rotation.power);

    Send(&pkt, sizeof(pkt));
}

void SendMotorStopPacket(void)
{
    packet_header_t pkt;
    packet_mkbasic(&pkt, CPT_MOTORSTOP);

    Send(&pkt, sizeof(pkt));
}

void SendKeepAlivePacket(void)
{
    packet_header_t pkt;
    packet_mkbasic(&pkt, PT_SYN);

    Send(&pkt, sizeof(pkt));
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

void setupCommunication(void)
{
    Serial.println("Initializing communication...");
    transmitter.begin(115200);
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

    setupCommunication();

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
