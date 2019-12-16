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
    \param[out] resultBuffer    Buffer to store he resulting command in. This must have enough space to store the result.
    \param[in]  command         The command to be prepended to the result buffer.
    \param[in]  commandLength   The length of the command.
    \param[in]  value           The additional part(parameters, varying string etc.) to be appended to the result buffer.
    \param[in]  valueLength     The length of the value.
    \return                     N/A.
    */
    static void GenerateCommand(char* const resultBuffer, const char* const command, const size_t commandLength, const char* const value, const size_t valueLength);
    
    /*! SendCommand().
    \fn SendCommand().
    \param[in]  command         The command to be sent.
    \param[in]  responseLength  The expected respnse length to wait for.
    \return                     A pointer to the resulting string. This is stored in an internal buffer and is not guaranteed to persist.
    */
    const char *SendCommand(const char *const command, const int responseLength);

public:
    /*! Ping().
    \fn Ping().
    \param  N/A.
    \return True is the device is responsive, false otherwise.
    */
    bool Ping(void);

    /*! GetVersion().
    \fn GetVersion().
    \param  N/A.
    \return A pointer to the resulting string containing the device's version. This is stored in an internal buffer and is not guaranteed to persist.
    */
    const char *GetVersion(void);
    
    /*! SetBaudRate().
    \fn SetBaudRate().
    \param[in]  value   The device's desirable baud rate.
    \return             True on success, false otherwise.
    */
    bool SetBaudRate(const HC06::baudrate_t value);
    
    /*! SetName().
    \fn SetName().
    \param[in]  value   The device's desirable name (max. 20 characters).
    \return             True on success, false otherwise.
    */
    bool SetName(const char *const value);
    
    /*! SetPIN().
    \fn SetPIN().
    \param[in]  value   The device's desirable pin code (max. 4 characters).
    \return             True on success, false otherwise.
    */
    bool SetPIN(const char *const value);
    
    /*! Begin().
    \fn Begin().
    \param[in]  baudRate    The desirable baud rate used to communicate with the device.
    \return                 N/A.
    */
    void Begin(const HC06::baudrate_t baudRate);
    
    /*! AvailableBytes().
    \fn AvailableBytes().
    \param  N/A.
    \return The number of bytes available for read.
    */
    int AvailableBytes(void);

    /*! Write().
    \fn Write().
    \param[in]  buffer  A sequence of any characters to be written to the device.
    \param[in]  length  The length of the buffer.
    \return             The number of bytes written.
    */
    size_t Write(const uint8_t *const buffer, const size_t length); /*!< TODO */

    /*! Write().
    \fn Write().
    \param[in]  buffer  A pointer to an object/buffer to be written to the device.
    \param[in]  length  The length of the buffer.
    \return             The number of bytes written.
    */
    size_t Write(const char *const buffer, const size_t length);

    /*! Write().
    \fn Write().
    \param[in]  str A null-terminated string to be written to the device.
    \return         The number of bytes written.
    */
    size_t Write(const char *const str);

    /*! Write().
    \fn Write().
    \param[in]  byte    A single byte to be written to the device.
    \return             The number of bytes written.
    */
    size_t Write(const uint8_t byte);
    
    /*! Flush().
    \fn Flush().
    \param  N/A.
    \return N/A.
    */
    void Flush(void);

    /*! operator bool().
    \fn operator bool().
    \param  N/A.
    \return True if the serial is available, false otherwise.
    */
    operator bool(void);

    /*! HC06().
    \fn HC06().
    \param[in]  rxPin           The pin number to read from the device.
    \param[in]  txPin           The pin number to write to the device.
    \param[in]  timeout         Communication timeout (Optional, default: 1000UL).
    \param[in]  inverseLogic    Inverses the logic (Optional, default: false).
    \return                     N/A.
    */
    HC06(const uint8_t rxPin, const uint8_t txPin, const unsigned long int timeout = 1000UL, const bool inverseLogic = false);
};

#endif // __HC06_HPP__
