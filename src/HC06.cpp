#include "HC06.hpp"
#include <Arduino.h>

/*
https://mcuoneclipse.com/2013/06/19/using-the-hc-06-bluetooth-module/

AT          OK              Used to verify communication
AT+VERSION  OKlinvorV1.8    The firmware version(version might depend on firmware)
AT+NAMExyz  OKsetname       Sets the module name to "xyz"
AT+PIN1234  OKsetPIN        Sets the module PIN to 1234
AT+BAUD1    OK1200          Sets the baud rate to 1200
AT+BAUD2    OK2400          Sets the baud rate to 2400
AT+BAUD3    OK4800          Sets the baud rate to 4800
AT+BAUD4    OK9600          Sets the baud rate to 9600
AT+BAUD5    OK19200         Sets the baud rate to 19200
AT+BAUD6    OK38400         Sets the baud rate to 38400
AT+BAUD7    OK57600         Sets the baud rate to 57600
AT+BAUD8    OK115200        Sets the baud rate to 115200
AT+BAUD9    OK230400        Sets the baud rate to 230400
AT+BAUDA    OK460800        Sets the baud rate to 460800
AT+BAUDB    OK921600        Sets the baud rate to 921600
AT+BAUDC    OK1382400       Sets the baud rate to 1382400
*/

#define STRLEN(x) ((sizeof(x) / sizeof(*(x))) - 1U)

#define HC06_AT                     "AT"
#define HC06_OK                     "OK"
#define HC06_PING_COMMAND           HC06_AT
#define HC06_PING_RESPONSE          HC06_OK
#define HC06_VERSION_COMMAND        HC06_AT "+VERSION"
#define HC06_NAME_COMMAND           HC06_AT "+NAME"
#define HC06_NAME_RESPONSE          HC06_OK "setname"
#define HC06_PIN_COMMAND            HC06_AT "+PIN"
#define HC06_PIN_RESPONSE           HC06_OK "setPIN"

#define HC06_BAUD1200_COMMAND       HC06_AT "+BAUD1"
#define HC06_BAUD1200_RESPONSE      HC06_OK "1200"
#define HC06_BAUD2400_COMMAND       HC06_AT "+BAUD2"
#define HC06_BAUD2400_RESPONSE      HC06_OK "2400"
#define HC06_BAUD4800_COMMAND       HC06_AT "+BAUD3"
#define HC06_BAUD4800_RESPONSE      HC06_OK "4800"
#define HC06_BAUD9600_COMMAND       HC06_AT "+BAUD4"
#define HC06_BAUD9600_RESPONSE      HC06_OK "9600"
#define HC06_BAUD19200_COMMAND      HC06_AT "+BAUD5"
#define HC06_BAUD19200_RESPONSE     HC06_OK "19200"
#define HC06_BAUD38400_COMMAND      HC06_AT "+BAUD6"
#define HC06_BAUD38400_RESPONSE     HC06_OK "38400"
#define HC06_BAUD57600_COMMAND      HC06_AT "+BAUD7"
#define HC06_BAUD57600_RESPONSE     HC06_OK "57600"
#define HC06_BAUD115200_COMMAND     HC06_AT "+BAUD8"
#define HC06_BAUD115200_RESPONSE    HC06_OK "115200"
#define HC06_BAUD230400_COMMAND     HC06_AT "+BAUD9"
#define HC06_BAUD230400_RESPONSE    HC06_OK "230400"
#define HC06_BAUD460800_COMMAND     HC06_AT "+BAUDA"
#define HC06_BAUD460800_RESPONSE    HC06_OK "460800"
#define HC06_BAUD921600_COMMAND     HC06_AT "+BAUDB"
#define HC06_BAUD921600_RESPONSE    HC06_OK "921600"
#define HC06_BAUD1382400_COMMAND    HC06_AT "+BAUDC"
#define HC06_BAUD1382400_RESPONSE   HC06_OK "1382400"

void HC06::GenerateCommand(char*const resultBuffer, const char* const command, const size_t commandLength, const char* const value, const size_t valueLength)
{
    strcpy(resultBuffer, command);
    strcpy(resultBuffer + commandLength, value);
    resultBuffer[commandLength + valueLength] = '\0';
}

const char *HC06::SendCommand(const char *const command, const int responseLength)
{
    unsigned long int timeout;
    int availableBytes;

    SoftwareSerial::print(command);
    SoftwareSerial::flush();

    timeout = millis() + m_Timeout;
    while((availableBytes = SoftwareSerial::available()) < responseLength)
    {
        if((long)(millis() - timeout) >= 0)
        {
            return nullptr;
        }
    }

    if((size_t)availableBytes > HC06::COMMANDBUFFER_SIZE)
    {
        return nullptr;
    }

    size_t readLen = SoftwareSerial::readBytes(m_CommandBuffer, availableBytes);
    if(readLen <= 0U || readLen > HC06::COMMANDBUFFER_SIZE)
    {
        return nullptr;
    }

    m_CommandBuffer[readLen] = '\0';
    return m_CommandBuffer;
}

bool HC06::Ping(void)
{
    const char *result = SendCommand(HC06_PING_COMMAND, STRLEN(HC06_PING_RESPONSE));
    return result != nullptr && strcmp(result, HC06_PING_RESPONSE) == 0;
}

const char *HC06::GetVersion(void)
{
    const char *result = SendCommand(HC06_VERSION_COMMAND, 12);
    return result != nullptr ? (result + STRLEN(HC06_OK)) : nullptr;
}

bool HC06::SetBaudRate(const HC06::baudrate_t value)
{
    const char *cmd;
    const char *rsp;

    switch(value)
    {
        case HC06::baudrate_t::BR_1200:
            cmd = HC06_BAUD1200_COMMAND;
            rsp = HC06_BAUD1200_RESPONSE;
            break;

        case HC06::baudrate_t::BR_2400:
            cmd = HC06_BAUD2400_COMMAND;
            rsp = HC06_BAUD2400_RESPONSE;
            break;

        case HC06::baudrate_t::BR_4800:
            cmd = HC06_BAUD4800_COMMAND;
            rsp = HC06_BAUD4800_RESPONSE;
            break;

        case HC06::baudrate_t::BR_9600:
            cmd = HC06_BAUD9600_COMMAND;
            rsp = HC06_BAUD9600_RESPONSE;
            break;

        case HC06::baudrate_t::BR_19200:
            cmd = HC06_BAUD19200_COMMAND;
            rsp = HC06_BAUD19200_RESPONSE;
            break;

        case HC06::baudrate_t::BR_38400:
            cmd = HC06_BAUD38400_COMMAND;
            rsp = HC06_BAUD38400_RESPONSE;
            break;

        case HC06::baudrate_t::BR_57600:
            cmd = HC06_BAUD57600_COMMAND;
            rsp = HC06_BAUD57600_RESPONSE;
            break;

        case HC06::baudrate_t::BR_115200:
            cmd = HC06_BAUD115200_COMMAND;
            rsp = HC06_BAUD115200_RESPONSE;
            break;

        case HC06::baudrate_t::BR_230400:
            cmd = HC06_BAUD230400_COMMAND;
            rsp = HC06_BAUD230400_RESPONSE;
            break;

        case HC06::baudrate_t::BR_460800:
            cmd = HC06_BAUD460800_COMMAND;
            rsp = HC06_BAUD460800_RESPONSE;
            break;

        case HC06::baudrate_t::BR_921600:
            cmd = HC06_BAUD921600_COMMAND;
            rsp = HC06_BAUD921600_RESPONSE;
            break;

        case HC06::baudrate_t::BR_1382400:
            cmd = HC06_BAUD1382400_COMMAND;
            rsp = HC06_BAUD1382400_RESPONSE;
            break;

        default:
            return false;
    }

    const char *result = SendCommand(cmd, strlen(rsp));
    return result != nullptr && strcmp(result, rsp) == 0;
}

bool HC06::SetName(const char *const value)
{
    size_t len = strlen(value);
    if(len > HC06::NAME_MAXLEN)
    {
        return false;
    }

    HC06::GenerateCommand(m_CommandBuffer, HC06_NAME_COMMAND, STRLEN(HC06_NAME_COMMAND), value, len);
    const char *result = SendCommand(m_CommandBuffer, STRLEN(HC06_NAME_RESPONSE));
    return result != nullptr && strcmp(result, HC06_NAME_RESPONSE) == 0;
}

bool HC06::SetPIN(const char *const value)
{
    size_t len = strlen(value);
    if(len != HC06::PIN_LEN)
    {
        return false;
    }

    HC06::GenerateCommand(m_CommandBuffer, HC06_PIN_COMMAND, STRLEN(HC06_PIN_COMMAND), value, len);
    const char *result = SendCommand(m_CommandBuffer, STRLEN(HC06_PIN_RESPONSE));
    return result != nullptr && strcmp(result, HC06_PIN_RESPONSE) == 0;
}

void HC06::Begin(const HC06::baudrate_t baudRate) { SoftwareSerial::begin((long int)baudRate); }
int HC06::AvailableBytes(void) { return SoftwareSerial::available(); }
size_t HC06::Write(const uint8_t *const buffer, const size_t length) { return SoftwareSerial::write(buffer, length); }
size_t HC06::Write(const char *const buffer, const size_t length) { return SoftwareSerial::write(buffer, length); }
size_t HC06::Write(const char *const str) { return SoftwareSerial::write(str); }
size_t HC06::Write(const uint8_t byte) { return SoftwareSerial::write(byte); }
void HC06::Flush(void) { SoftwareSerial::flush(); }

HC06::operator bool(void) { return SoftwareSerial::operator bool(); }

HC06::HC06(const uint8_t rxPin, const uint8_t txPin, const unsigned long int timeout, const bool inverseLogic) : SoftwareSerial(rxPin, txPin, inverseLogic)
{
    m_Timeout = timeout;
}
