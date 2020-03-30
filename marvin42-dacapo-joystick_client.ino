#include <math.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
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
#include "debug.hpp"

#define PIN_JS1_VRX         A0
#define PIN_JS1_VRY         A1
#define PIN_JS1_SW          6

#define PIN_JS2_VRX         A2
#define PIN_JS2_VRY         A3
#define PIN_JS2_SW          7

#define PIN_SERIAL_RX       11
#define PIN_SERIAL_TX       12

#define PIN_SPEEDREGULATOR  A6

#define PIN_NAVMODE         8

#define KEEPALIVE_INTERVAL  (1000UL - 50UL)

#define LOOP_DELAY          10UL

SoftwareSerial transmitter(PIN_SERIAL_RX, PIN_SERIAL_TX);

#define COM_BLUETOOTH
#ifdef COM_BLUETOOTH
char commandBuffer[27U + 1U];
StreamCommandHandler commandHandler(transmitter, commandBuffer);
#endif

Joystick leftJoystick(PIN_JS1_VRX, PIN_JS1_VRY, PIN_JS1_SW, 0.1f, 0.025f);
Joystick rightJoystick(PIN_JS2_VRX, PIN_JS2_VRY, PIN_JS2_SW, 0.1f, 0.025f);
Regulator speedRegulator(PIN_SPEEDREGULATOR, 0.1f, 0.9f, 0.1f);

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
    } spin;

    Quaternion rotation;
} inputdata = { { { 0.0f, 0.0f }, 0.0f}, { 0, 0.0f }, Quaternion() };

unsigned long int nextKeepAlive = 0UL;

void Send(const void* const data, const size_t size)
{
    transmitter.write((const uint8_t*)data, size);
    transmitter.flush();

    nextKeepAlive = millis() + KEEPALIVE_INTERVAL;
}

void SendDirectionPacket(void)
{
    packet_direction_t pkt;
    packet_mkdirection(&pkt, &inputdata.movement.direction);

    Send(&pkt, sizeof(pkt));
}

void SendDirQuatPacket(void)
{
    packet_dirquat_t pkt;
    quaterniondata_t q = { inputdata.rotation.w, inputdata.rotation.x, inputdata.rotation.y, inputdata.rotation.z };
    packet_mkdirquat(&pkt, &inputdata.movement.direction, &q);

    Send(&pkt, sizeof(pkt));
}

void SendMotorPowerPacket(void)
{
    packet_motorpower_t pkt;
    packet_mkmotorpower(&pkt, inputdata.movement.power);

    Send(&pkt, sizeof(pkt));
}

void SendMotorSpinPacket(void)
{
    packet_motorspin_t pkt;
    packet_mkmotorspin(&pkt, inputdata.spin.direction, inputdata.spin.power);

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
    DebugPrintN(DM_JSLEFT, "Joystick(Left): "); DebugPrintN(DM_JSLEFT, !value ? "Pressed" : "Released");
    DebugPrintLineN(DM_JSLEFT);

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

    if(digitalRead(PIN_NAVMODE) == HIGH)
    {
        SendDirQuatPacket();
    }
    else
    {
        SendDirectionPacket();
    }

    DebugPrintN(DM_JSLEFT, "Joystick(Left): ");
    DebugPrintN(DM_JSLEFT, "x="); DebugPrintN(DM_JSLEFT, inputdata.movement.direction.x); DebugPrintN(DM_JSLEFT, ", ");
    DebugPrintN(DM_JSLEFT, "y="); DebugPrintN(DM_JSLEFT, inputdata.movement.direction.y);
    DebugPrintLineN(DM_JSLEFT);
}

void onLeftJoystickAxisChanged2(float x, float y)
{
    x = clamp11((float)roundf(-x));
    y = clamp11((float)roundf(-y));

    if(x == inputdata.movement.direction.x && y == inputdata.movement.direction.y)
        return;

    inputdata.movement.direction.x = x;
    inputdata.movement.direction.y = y;

    if(digitalRead(PIN_NAVMODE) == HIGH)
    {
        SendDirQuatPacket();
    }
    else
    {
        SendDirectionPacket();
    }

    DebugPrintN(DM_JSLEFT, "Joystick(Left): ");
    DebugPrintN(DM_JSLEFT, "x="); DebugPrintN(DM_JSLEFT, inputdata.movement.direction.x); DebugPrintN(DM_JSLEFT, ", ");
    DebugPrintN(DM_JSLEFT, "y="); DebugPrintN(DM_JSLEFT, inputdata.movement.direction.y);
    DebugPrintLineN(DM_JSLEFT);
}

void onRightJoystickButtonPressed(const bool value)
{
    DebugPrintN(DM_JSRIGHT, "Joystick(Right): "); DebugPrintN(DM_JSRIGHT, !value ? "Pressed" : "Released");
    DebugPrintLineN(DM_JSRIGHT);

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
        if(inputdata.spin.direction != 0)
        {
            inputdata.spin.direction = 0;
            SendMotorStopPacket();
        }

        return;
    }

    x = clamp11((float)roundf(-x));
    //y = clamp11((float)roundf(-y));

    if(x == inputdata.spin.direction)
        return;

    inputdata.spin.direction = x;
    inputdata.spin.power = 1.0f;

    SendMotorSpinPacket();

    DebugPrintN(DM_JSRIGHT, "Joystick(Right): ");
    DebugPrintN(DM_JSRIGHT, "x="); DebugPrintN(DM_JSRIGHT, x); DebugPrintN(DM_JSRIGHT, ", ");
    DebugPrintN(DM_JSRIGHT, "y="); DebugPrintN(DM_JSRIGHT, y);
    DebugPrintLineN(DM_JSRIGHT);
}

void onSpeedRegulated(const float oldValue, const float newValue)
{
    inputdata.movement.power = newValue;

    SendMotorPowerPacket();

    DebugPrintN(DM_SPEED, "Speed: ");
    DebugPrintN(DM_SPEED, (newValue - oldValue) > 0.0f ? "Increased to " : "Decreased to "); DebugPrintN(DM_SPEED, inputdata.movement.power);
    DebugPrintLineN(DM_SPEED);
}

void setupFail(void)
{
    while(true);
}

#define MPU6060_INT_PIN 2
MPU6050 mpu;
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
    mpuInterrupt = true;
}

void setupMPU6050(void)
{
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

    // initialize device
    DebugPrintLineN(DM_SETUP, "MPU6050...");
    mpu.initialize();
    pinMode(MPU6060_INT_PIN, INPUT);

    if(!mpu.testConnection())
    {
        DebugPrintLineN(DM_SETUP, "Failed");
        setupFail();
    }

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    //mpu.PrintActiveOffsets();

    // load and configure the DMP
    devStatus = mpu.dmpInitialize();
    if(devStatus != 0)
    {
        DebugPrintN(DM_SETUP, "Fail: DMP, "); DebugPrintLineN(DM_SETUP, devStatus);
        setupFail();
    }

    // turn on the DMP, now that it's ready
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    attachInterrupt(digitalPinToInterrupt(MPU6060_INT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
}

bool readDMP(void)
{
    return (mpu.getIntStatus() & 0b00001) != 0 && mpu.dmpGetCurrentFIFOPacket(fifoBuffer) != 0 && mpu.dmpGetQuaternion(&inputdata.rotation, fifoBuffer) == 0;
}

void setupCommunication(void)
{
    DebugPrintLineN(DM_SETUP, "Initializing communication...");
    transmitter.begin(115200);
}

void setup(void)
{
    delay(2500);
    Serial.begin(9600, SERIAL_8N1);
    DebugPrintLineN(DM_SETUP, "Initializing...");

    pinMode(PIN_NAVMODE, INPUT);

    DebugPrintLineN(DM_SETUP, "Initializing input device...");
    leftJoystick.SetOnAxisChangedEvent(onLeftJoystickAxisChanged2);
    leftJoystick.SetOnStateChangedEvent(onLeftJoystickButtonPressed);
    rightJoystick.SetOnAxisChangedEvent(onRightJoystickAxisChanged);
    leftJoystick.SetOnStateChangedEvent(onRightJoystickButtonPressed);
    speedRegulator.SetOnValueChangedEvent(onSpeedRegulated);

    setupCommunication();

    setupMPU6050();

    DebugPrintLineN(DM_SETUP, "Done");
    DebugPrintLineN(DM_SETUP);
    DebugFlushN(DM_SETUP);
}

void loop(void)
{
    leftJoystick.Poll();
    //rightJoystick.Poll();
    speedRegulator.Poll();

    if((long)(millis() - nextKeepAlive) >= 0L)
    {
        SendKeepAlivePacket();
    }

    readDMP();

    delay(LOOP_DELAY);
}
