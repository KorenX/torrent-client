#ifndef __SERVER_PROTOCOL_MESSAGES_H__
#define __SERVER_PROTOCOL_MESSAGES_H__

#include <General/DataStructures.h>

namespace ServerProtocol
{
    // enums and consts
    enum class MessageTypes: uint8_t
    {
        FILES_LIST = 1,
        FILES_CHUNK,
        FILES_ACK,
        FILES_FIN,
        PEERS_LIST,
        PEERS_CHUNK,
        PEERS_ACK,
        PEERS_FIN,
        THANKS
    };

    static constexpr size_t MAX_FILES_INFO_IN_MESSAGE = 8;
    static constexpr size_t MAX_PEERS_INFO_IN_MESSAGE = 8;

    // message structures
#pragma pack(push, 1)
    struct MessageHeader
    {
        MessageTypes message_type;
    };

    typedef MessageHeader FilesListMessage;

    struct FilesChunkMessage
    {
        MessageHeader message_header;
        DataSturtures::FileInfo files_info[MAX_FILES_INFO_IN_MESSAGE];
    };

    struct AckMessage
    {
        MessageHeader message_header;
        uint32_t ack_index;
    };

    typedef AckMessage FilesAckMessage;
    typedef MessageHeader FilesFinishMessage;

    typedef MessageHeader PeersListMessage;

    struct PeersChunkMessage
    {
        MessageHeader message_header;
        DataSturtures::PeerInfo peers_info[MAX_PEERS_INFO_IN_MESSAGE];
    };

    typedef AckMessage PeersAckMessage;
    typedef MessageHeader PeersFinishMessage;

    typedef MessageHeader ThanksMessage;

#pragma pack(pop)
}

#endif //__SERVER_PROTOCOL_MESSAGES_H__