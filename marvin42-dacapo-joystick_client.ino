#include <math.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "src/crc.h"
#include "src/packet.h"
#include "src/custom_packets.h"
#include "src/Vector2.hpp"
#include "InterruptIn.hpp"
#include "src/generic.hpp"

#define STASSID ""
#define STAPSK  ""

unsigned long delayTime = 500;
const unsigned int MAX = 1024 + 1;

WiFiUDP client;

#define ADDRESS "192.168.10.33"
#define PORT    1042

InterruptIn joystickButton(D2);
InterruptIn pauseButton(D3);

void onJoystickButtonPressed(const bool value)
{
    if(value)
    {
        Serial.println("Joystick button released");
    }
    else
    {
        Serial.println("Joystick button pressed");
    }
}

bool isPaused = false;
void onPauseButtonPressed(const bool value)
{
    if(!value)
        return;

    isPaused = !isPaused;
    Serial.println(isPaused ? "Paused" : "Unpaused");
    delay(150);
}

void setup(void)
{
    Serial.begin(115200);

    //randomSeed(analogRead(0));//*

    // Initialize the output variables as outputs
    pinMode(A0, INPUT); // X axis
    pinMode(D1, INPUT); // Y axis
    pinMode(D8, INPUT); //Inbuilt 10k pulldown
    pinMode(D3, INPUT); //Inbuilt 10k pullup

    pinMode(D2, INPUT);
    digitalWrite(D2, HIGH);

    /*WiFi.mode(WIFI_STA);
    WiFi.begin(STASSID, STAPSK);
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);
    }

    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    client.begin(PORT);*/

    joystickButton.SetOnStateChangedCallback(onJoystickButtonPressed);
    pauseButton.SetOnStateChangedCallback(onPauseButtonPressed);
}

static float DotNormalized(const Vector2& a, const Vector2& b)
{
    return (asin(Vector2::DotProduct(a.GetNormalized(), b.GetNormalized())) / M_PI) * 2.0f;
}

static Vector2 CrossNormalized(const Vector2& a, const Vector2& b)
{
    return Vector2::PerpendicularCW(a.GetNormalized() - b.GetNormalized()).GetNormalized();
}

static Vector2 Deviation(const Vector2& position)
{
    return Vector2
    (
        -CrossNormalized(Vector2::Zero, position).GetY(),
        clamp11(DotNormalized(Vector2::Zero, position))
    );
}

void loop(void)
{
    /*pauseButton.Poll();
    if(isPaused)
        return;
    */
    //int axisX = random(0, MAX); // Debug
    //int axisY = random(0, MAX); // Debug
    int axisX = analogRead(A0);
    int axisY = analogRead(D1);
    int axisYF = digitalRead(D8);
    int axisYB = digitalRead(D3);
    float t; 

    //Button controll for forward and backward drive.
    if(axisYF){
        t = 1;
    }else if(!axisYB){
        t = -1;
    } else {
        t = 0;
    }
    //Vector2 norm = Vector2(normalize11((float)axisX, 0, MAX), normalize11((float)axisY, 0, MAX);
    Vector2 norm = Vector2(normalize11((float)axisX, 0, MAX), t);

    /*if(abs(norm.GetX()) <= 0.01f && abs(norm.GetY()) <= 0.01f)
    {
        Serial.println("Deadzone");
        return;
    }*/

    Serial.print("x="); Serial.print(norm.GetX()); Serial.print(", y="); Serial.println(norm.GetY());
    joystickButton.Poll();

    /*packet_motorrun_t pkt;
    packet_mkmotorrun(&pkt, normX, normY);

    client.beginPacket(ADDRESS, PORT);
    client.write((const char*)&pkt, sizeof(pkt));
    client.endPacket();*/

    delay(delayTime);
}
