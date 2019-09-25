#include <math.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "src/crc.h"
#include "src/packet.h"
#include "src/custom_packets.h"
#include "InterruptIn.hpp"
#include "RotaryEncoder.hpp"
#include "src/generic.hpp"

#define STASSID ""
#define STAPSK  ""

unsigned long delayTime = 500;
const unsigned int MAX = 1024 + 1;

WiFiUDP client;

#define ADDRESS "192.168.10.33"
#define PORT    1042

//InterruptIn pauseButton(D3);
RotaryEncoder rotaryEncoder(D0, D1, D4);
int currentCount = 0;

int8_t direction = 0;
float balance = 0.0f;
float lastBalance = 0.0f;
int8_t lastDirection = -2;

void onJoystickButtonPressed(const bool value)
{
    if(value)
    {
        //Serial.println("Joystick button released");
        Serial.println("Reset balance");
        currentCount = 0;
    }
    else
    {
        //Serial.println("Joystick button pressed");
    }

    //delay(100);
}

void onRotationChanged(const int value, const bool cw)
{
    if(cw)
    {
        if(currentCount >= RotaryEncoder::MaxCount)
        {
            Serial.println("Max. value reached: ");
            return;
        }
    }
    else
    {
        if(currentCount <= -RotaryEncoder::MaxCount)
        {
            Serial.println("Min. value reached: ");
            return;
        }
    }

    currentCount += cw ? 1 : -1;
    //Serial.print("Counter: "); Serial.print(currentCount); Serial.print(", CW: "); Serial.println(cw);
    //delay(100);
}

void onRevolution(const bool cw)
{
    //currentCount = cw ? RotaryEncoder::MaxCount : -RotaryEncoder::MaxCount;
}

/*bool isPaused = false;
void onPauseButtonPressed(const bool value)
{
    if(!value)
        return;

    isPaused = !isPaused;
    Serial.println(isPaused ? "Paused" : "Unpaused");
    delay(150);
}*/

uint8_t lastCLKState;
void setup(void)
{
    Serial.begin(115200);

    //randomSeed(analogRead(0));//*

    // Initialize the output variables as outputs
    pinMode(D8, INPUT); // Inbuilt 10k pulldown
    pinMode(D3, INPUT); // Inbuilt 10k pullup

    WiFi.mode(WIFI_STA);
    WiFi.begin(STASSID, STAPSK);
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);
    }

    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    client.begin(PORT);

    rotaryEncoder.SetOnValueChangedEvent(onRotationChanged);
    rotaryEncoder.SetOnRevolutionEvent(onRevolution);
    rotaryEncoder.SetOnStateChangedEvent(onJoystickButtonPressed);
    //pauseButton.SetOnStateChangedEvent(onPauseButtonPressed);
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
    balance *= -1;
    //Serial.print("balance="); Serial.print(balance); Serial.print(", direction="); Serial.println(direction);

    if(absdiff(balance, lastBalance) <= 0.05f && direction == lastDirection)
    {
        //Serial.println("Deadzone");
        return;
    }

    lastBalance = balance;
    lastDirection = direction;

    Serial.println("Sending data");
    SendMotorJSDataPacket(balance, direction);

    //delay(delayTime);
}
