#ifndef __HC06_HPP__
#define __HC06_HPP__

#include <stddef.h>
#include <SoftwareSerial.h>

class HC06 : private SoftwareSerial
{
public:
    static const size_t NAME_MAXLEN = 20U; /*!< TODO */
    static const size_t PIN_LEN = 4U; /*!< TODO */

    typedef enum
    {
        BR_1200 = 1200,
        BR_2400 = 2400,
        BR_4800 = 4800,
        BR_9600 = 9600,
        BR_19200 = 19200,
        BR_38400 = 38400,
        BR_57600 = 57600,
        BR_115200 = 115200,
        BR_230400 = 230400,
        BR_460800 = 460800,
        BR_921600 = 921600,
        BR_1382400 = 1382400
    } baudrate_t;

private:
    static const size_t COMMANDBUFFER_SIZE = (((2U + 5U) + NAME_MAXLEN) + 1U); // Largest size a command/response can be (including null character); HC06_NAME_COMMAND + NAME_MAXLEN + 1

    char m_CommandBuffer[HC06::COMMANDBUFFER_SIZE]; /*!< TODO */
    unsigned long int m_Timeout; /*!< TODO */
    
    /*! GenerateCommand().
    \fn GenerateCommand().
    \param .
    \return .
    */
    static void GenerateCommand(char* const resultBuffer, const char* const command, const size_t commandLength, const char* const value, const size_t valueLength);
    
    /*! SendCommand().
    \fn SendCommand().
    \param .
    \return .
    */
    const char *SendCommand(const char *const command, const int responseLength);

public:
    /*! Ping().
    \fn Ping().
    \param .
    \return .
    */
    bool Ping(void);

    /*! GetVersion().
    \fn GetVersion().
    \param .
    \return .
    */
    const char *GetVersion(void);
    
    /*! SetBaudRate().
    \fn SetBaudRate().
    \param .
    \return .
    */
    bool SetBaudRate(const HC06::baudrate_t value);
    
    /*! SetName().
    \fn SetName().
    \param .
    \return .
    */
    bool SetName(const char *const value);
    
    /*! SetPIN().
    \fn SetPIN().
    \param .
    \return .
    */
    bool SetPIN(const char *const value);
    
    /*! Begin().
    \fn Begin().
    \param .
    \return .
    */
    void Begin(const HC06::baudrate_t baudRate);
    
    /*! AvailableBytes().
    \fn AvailableBytes().
    \param .
    \return .
    */
    int AvailableBytes(void);
    size_t Write(const uint8_t *const buffer, const size_t length); /*!< TODO */
    size_t Write(const char *const buffer, const size_t length); /*!< TODO */
    size_t Write(const char *const str); /*!< TODO */
    size_t Write(const uint8_t byte); /*!< TODO */
    
    /*! Flush().
    \fn Flush().
    \param .
    \return .
    */
    void Flush(void);

    operator bool(void);

    HC06(const uint8_t rxPin, const uint8_t txPin, const unsigned long int timeout = 1000UL, const bool inverseLogic = false);
};

#endif // __HC06_HPP__
