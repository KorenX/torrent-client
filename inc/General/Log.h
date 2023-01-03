#ifndef __GENERAL_LOG_H__
#define __GENERAL_LOG_H__

#include <stddef.h>

namespace Logging
{
    void Log(const char* format, ...);

    void PrintBuffer(void* buffer, size_t buffer_size);

    static constexpr size_t LINE = 16;
}

#define PRINT Logging::Log

#endif //__GENERAL_LOG_H__