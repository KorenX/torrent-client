#ifndef __GENERAL_DATA_STRUCTURES_H__
#define __GENERAL_DATA_STRUCTURES_H__

#include <stdint.h>
#include <stddef.h>

namespace DataStructures
{
    static constexpr size_t MAX_FILE_NAME_LENGTH = 32;
    static constexpr size_t MAX_FILE_DESC_LENGTH = 64;

#pragma pack(push, 1)
    struct FileInfo
    {
        uint32_t file_id;
        char file_name[MAX_FILE_NAME_LENGTH];
        char file_desc[MAX_FILE_DESC_LENGTH];
    };

    struct PeerInfo
    {
        uint32_t peer_ip;
    };
#pragma pack(pop)
}

#endif //__GENERAL_DATA_STRUCTURES_H__