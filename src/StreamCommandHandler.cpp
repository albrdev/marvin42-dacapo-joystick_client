#include "StreamCommandHandler.hpp"
#include <string.h>
#include <Arduino.h>

#define STRLEN(x) ((sizeof(x) / sizeof(*(x))) - 1U)

size_t StreamCommandHandler::AwaitResponse(const size_t length, const unsigned long int timeout)
{
    size_t result;
    unsigned long int timeoutPoint = millis() + timeout;

    while((result = (size_t)m_Source.available()) < length && (long)(millis() - timeoutPoint) < 0);

    return result;
}

char* StreamCommandHandler::GetResponse(const size_t length, const unsigned long int timeout)
{
    size_t availableSize = AwaitResponse(length, timeout);

    // Response is somehow longer than the buffer can handle, this should never happen
    if(availableSize > length)
    {
        return nullptr;
    }

    size_t readLength = m_Source.readBytes(m_Buffer, availableSize); // Receive the actual response
    if(readLength != availableSize) // Extra check to see if we got the length (probably unnecessary)
    {
        return nullptr;
    }

    m_Buffer[readLength] = '\0';
    return m_Buffer;
}

bool StreamCommandHandler::PrintCommand(const char* const command, const char* const response, const unsigned long int timeout)
{
    m_Source.print(command); // Send command to device
    m_Source.flush();        // Flush immediately

    size_t responseLength = strlen(response);
    char* responseResult = GetResponse(responseLength, timeout);

    return responseResult != nullptr && strcmp(response, responseResult) == 0;
}

size_t StreamCommandHandler::Print(const char* const str)
{
    return m_Source.print(str);
}

size_t StreamCommandHandler::PrintLine(const char* const str)
{
    return m_Source.println(str);
}

StreamCommandHandler::StreamCommandHandler(Stream& source, char* const buffer) : m_Source(source), m_Buffer(buffer) { }
