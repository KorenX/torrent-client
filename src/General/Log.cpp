#include <General/Log.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

namespace Logging
{
    void Log(const char* format, ...)
    {
        va_list vl;
        va_start(vl, format);
        vprintf(format, vl);
        va_end(vl);
    }

    void PrintBuffer(void* buffer, size_t buffer_size)
    {
        uint8_t* buff = reinterpret_cast<uint8_t*>(buffer);
        for (size_t i = 0; i < buffer_size; i++)
        {
            printf("%02x, ", buff[i]);
            if (i % LINE == LINE - 1)
            {
                printf("\n");
            }
        }

        if (buffer_size % LINE != LINE - 1)
        {
            printf("\n");
        }
    }
}
