#ifndef __NETWORKS_STATUS_H__
#define __NETWORKS_STATUS_H__

#include <stddef.h>

namespace Networks
{
    enum class Status
    {
        Success = 0,
        SocketCreationFailed,
        NotInitialized,
        InvalidNullptr,
        ReadFailed,
        WriteFailed,
        PartiallyWritten,
        SetTimeoutFailed,
    };

    static constexpr size_t MILISECONDS_IN_SECONDS = 1000;
}

#endif //__NETWORKS_STATUS_H__