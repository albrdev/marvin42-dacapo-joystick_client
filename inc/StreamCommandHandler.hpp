#ifndef __STREAMCOMMANDHANDLER_HPP__
#define __STREAMCOMMANDHANDLER_HPP__

#include <stddef.h>
#include <Stream.h>

#define SCH_TIMEOUT_DEFAULT (1U * 1000U)

class StreamCommandHandler
{
private:
    Stream& m_Source;
    char* m_Buffer;

public:
    size_t AwaitResponse(const size_t length, const unsigned long int timeout = SCH_TIMEOUT_DEFAULT);
    char* GetResponse(const size_t length, const unsigned long int timeout = SCH_TIMEOUT_DEFAULT);
    bool PrintCommand(const char* const command, const char* const response, const unsigned long int timeout = SCH_TIMEOUT_DEFAULT);

    size_t Print(const char* const str);
    size_t PrintLine(const char* const str);

    StreamCommandHandler(Stream& source, char* const buffer);
};

#endif // __STREAMCOMMANDHANDLER_HPP__
