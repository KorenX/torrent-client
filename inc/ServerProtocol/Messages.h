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
    static constexpr size_t MAX_PEERS_INFO_IN_MESSAGE = 128;
    static constexpr size_t MAX_PACKET_SIZE = 1248;

    // message structures
#pragma pack(push, 1)
    struct MessageHeader
    {
        MessageTypes message_type;
    };

    using FilesListMessage=MessageHeader;

    struct FilesChunkMessage
    {
        MessageHeader message_header;
        uint32_t files_info_amount;
        uint32_t starting_index;
        DataStructures::FileInfo files_info[MAX_FILES_INFO_IN_MESSAGE];
        static constexpr size_t MINIMAL_SIZE = sizeof(message_header) + sizeof(files_info_amount) + sizeof(starting_index) + sizeof(DataStructures::FileInfo);
    };

    struct AckMessage
    {
        MessageHeader message_header;
        uint32_t ack_index;
    };

    using FilesAckMessage = AckMessage;
    using FilesFinishMessage = MessageHeader;

    struct PeersListMessage
    {
        MessageHeader message_header;
        DataStructures::FileInfo wanted_file;
    };
    
    struct PeersChunkMessage
    {
        MessageHeader message_header;
        uint32_t peers_info_amount;
        uint32_t starting_index;
        DataStructures::PeerInfo peers_info[MAX_PEERS_INFO_IN_MESSAGE];
        static constexpr size_t MINIMAL_SIZE = sizeof(message_header) + sizeof(peers_info_amount) + sizeof(starting_index) + sizeof(DataStructures::PeerInfo);
    };

    using PeersAckMessage = AckMessage;
    using PeersFinishMessage = MessageHeader;

    using ThanksMessage = MessageHeader;

    struct MaxServerMessage
    {
        MessageHeader message_header;
        uint8_t message_payload[MAX_PACKET_SIZE - sizeof(MessageHeader)];
    };

#pragma pack(pop)
    static_assert(MAX_PACKET_SIZE >= sizeof(FilesListMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(FilesChunkMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(FilesAckMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(FilesFinishMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(PeersListMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(PeersChunkMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(PeersAckMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(PeersFinishMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(ThanksMessage), "Message needs to be smaller than the max packet size");
    static_assert(MAX_PACKET_SIZE >= sizeof(MaxServerMessage), "Message needs to be smaller than the max packet size");
}

#endif //__SERVER_PROTOCOL_MESSAGES_H__