#include <Arduino.h>
#include "src/crc.h"
#include "src/packet.h"
#include "src/custom_packets.h"
#include "InterruptIn.hpp"
#include "src/generic.hpp"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "password"
#endif

unsigned long delayTime = 100;
const unsigned int MAX = 1024 + 1;

WiFiUDP client;

#define ADDRESS "192.168.10.33"
#define PORT    1042

InterruptIn pauseButton(D3);

bool isPaused = false;
void onPushPauseButton(const bool value)
{
    if(!value)
        return;

    isPaused = !isPaused;
    Serial.println(isPaused ? "Paused" : "Unpaused");
}

void setup()
{
    Serial.begin(115200);

    randomSeed(analogRead(0));//*

    // Initialize the output variables as outputs
    pinMode(A0, INPUT);
    pinMode(RX, INPUT);

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

    pauseButton.SetOnStateChangedCallback(onPushPauseButton);
}

void loop()
{
    pauseButton.Poll();
    if(isPaused)
        return;

    //int axisX = random(0, MAX); // Debug
    //int axisY = random(0, MAX); // Debug
    int axisX = analogRead(A0);
    int axisY = analogRead(RX);

    float normX = normalize11((float)axisX, 0, MAX);
    float normY = normalize11((float)axisY, 0, MAX);

    Serial.print("x="); Serial.print(normX);
    Serial.print(", y="); Serial.println(normY);

    packet_motorrun_t pkt;
    packet_mkmotorrun(&pkt, normX, normY);

    client.beginPacket(ADDRESS, PORT);
    client.write((const char*)&pkt, sizeof(pkt));
    client.endPacket();

    delay(delayTime);
}
