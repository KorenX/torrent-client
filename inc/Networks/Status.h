#ifndef __NETWORKS_STATUS_H__
#define __NETWORKS_STATUS_H__

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
    };
}

#endif //__NETWORKS_STATUS_H__