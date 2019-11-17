#include <math.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "crc.h"
#include "packet.h"
#include "custom_packets.h"
#include "InterruptIn.hpp"
#include "RotaryEncoder.hpp"

//#define M42_DEBUG // Must define before including 'generic.hpp'
#include "generic.hpp"

#include "config.h" // IP/port, WiFi SSID/password

const unsigned long delayTime = 500;

WiFiUDP client;
RotaryEncoder rotaryEncoder(D0, D1, D4);
//InterruptIn pauseButton(D3);

int currentCount = 0;
int8_t direction = 0;
float balance = 0.0f;
float lastBalance = 0.0f;
int8_t lastDirection = -2;

//bool isPaused = false;

void onJoystickButtonPressed(const bool value)
{
    if(value)
    {
        //PrintDebugLine("Joystick button released");
        PrintDebugLine("Reset balance");
        currentCount = 0;
    }
    /*else
    {
        //PrintDebugLine("Joystick button pressed");
    }*/

    //delay(100);
}

void onRotationChanged(const int value, const bool cw)
{
    if(cw)
    {
        if(currentCount >= RotaryEncoder::MaxCount)
        {
            PrintDebugLine("Max. value reached: ");
            return;
        }
    }
    else
    {
        if(currentCount <= -RotaryEncoder::MaxCount)
        {
            PrintDebugLine("Min. value reached: ");
            return;
        }
    }

    currentCount += cw ? -1 : 1;
    PrintDebug("Counter: "); PrintDebug(currentCount); PrintDebug(", Direction: "); PrintDebugLine(cw ? "CW" : "CCW");
    //delay(100);
}

/*void onPauseButtonPressed(const bool value)
{
    if(!value)
        return;

    isPaused = !isPaused;
    PrintDebugLine(isPaused ? "Paused" : "Unpaused");
    delay(150);
}*/

void setup(void)
{
    delay(2500);
    Serial.print("Initializing...");
    Serial.begin(115200);

    //randomSeed(analogRead(0));//*

    // Initialize the output variables as outputs
    pinMode(D8, INPUT); // Inbuilt 10k pulldown
    pinMode(D3, INPUT); // Inbuilt 10k pullup

    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PSK);
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);
    }

    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    client.begin(PORT);

    rotaryEncoder.SetOnValueChangedEvent(onRotationChanged);
    rotaryEncoder.SetOnStateChangedEvent(onJoystickButtonPressed);
    //pauseButton.SetOnStateChangedEvent(onPauseButtonPressed);

    Serial.println("Done");
}

void SendMotorJSDataPacket(const float balance, const float direction)
{
    packet_motorjsdata_t pkt;
    packet_mkmotorjsdata(&pkt, balance, direction);

    client.beginPacket(ADDRESS, PORT);
    client.write((const char*)&pkt, sizeof(pkt));
    client.endPacket();
}

void SendMotorStopPacket(void)
{
    packet_motorjsdata_t pkt;
    packet_mkbasic(&pkt, CPT_MOTORSTOP);

    client.beginPacket(ADDRESS, PORT);
    client.write((const char*)&pkt, sizeof(pkt));
    client.endPacket();
}

void loop(void)
{
    /*pauseButton.Poll();
    if(isPaused)
        return;*/

    uint8_t forwardsButton = digitalRead(D8);
    uint8_t backwardsButton = digitalRead(D3);

    if(forwardsButton)
        direction = 1;
    else if(!backwardsButton)
        direction = -1;

    rotaryEncoder.Poll();
    balance = normalize11((float)currentCount, -RotaryEncoder::MaxCount, RotaryEncoder::MaxCount);
    //PrintDebug("Input: "); PrintDebug("balance="); PrintDebug(balance); PrintDebug(", direction="); PrintDebugLine(direction);

    if(absdiff(balance, lastBalance) <= 0.05f && direction == lastDirection)
    {
        //Serial.println("Deadzone");
        return;
    }

    PrintDebug("Sending: "); PrintDebug("balance="); PrintDebug(balance); PrintDebug(", direction="); PrintDebugLine(direction);
    SendMotorJSDataPacket(balance, direction);

    lastBalance = balance;
    lastDirection = direction;
    //delay(delayTime);
}
