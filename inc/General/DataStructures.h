#include <stdint.h>

namespace DataSturtures
{
    static constexpr size_t MAX_FILE_NAME_LENGTH = 64;
    static constexpr size_t MAX_FILE_DESC_LENGTH = 64;

// #pragma pack(push, 1)
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
// #pragma pack(pop)
}
