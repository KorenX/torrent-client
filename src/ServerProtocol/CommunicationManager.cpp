#include <ServerProtocol/CommunicationManager.h>
#include <stdio.h>
#include <string.h>
#include <General/Log.h>

#define CHECK_RESULT(res) if (res != Results::Success) return res;

namespace ServerProtocol
{
    CommunicationManager::Results CommunicationManager::PrintAvailableFiles(bool print_desc)
    {
        Results res = SendFilesList();
        CHECK_RESULT(res);

        MaxServerMessage recv_msg;
        size_t recv_size = 0;
        Networks::Status recv_result = m_server_socket.Receive(&recv_msg, sizeof(recv_msg), recv_size);
        if (recv_result != Networks::Status::Success)
        {
            PRINT("Socket reading failed with error %u\n", static_cast<uint32_t>(recv_result));
            return Results::ReceivingFailed;
        }

        PRINT("FILE ID\t\tFILE NAME");
        print_desc ? PRINT("\tFILE DESC\n") : PRINT("\n");
        while (recv_msg.message_header.message_type == MessageTypes::FILES_CHUNK)
        {
            if (recv_size < FilesChunkMessage::MINIMAL_SIZE)
            {
                PRINT("Tried to construct files chunk message with %lu bytes\n", recv_size);
                return Results::ReceivedBadMessageSize;
            }

            res = HandleFilesChunk(reinterpret_cast<FilesChunkMessage&>(recv_msg), print_desc);
            CHECK_RESULT(res);

            recv_result = m_server_socket.Receive(&recv_msg, sizeof(recv_msg), recv_size);
            if (recv_result != Networks::Status::Success)
            {
                PRINT("Socket reading failed with error %u\n", static_cast<uint32_t>(recv_result));
                return Results::ReceivingFailed;
            }
        }

        PRINT("-------------------------\n");

        return (recv_msg.message_header.message_type == MessageTypes::FILES_FIN) ? Results::Success : Results::CouldNotGetAllFiles;
    }

    CommunicationManager::Results CommunicationManager::GetPeerForFile(const DataStructures::FileInfo& wanted_file, DataStructures::PeerInfo& o_peer)
    {
        if (m_finished_peers)
        {
            return Results::MaxPeersReached;
        }

        if (m_current_pi_index == 0)
        {
            Results res = SendPeersList(wanted_file);
            CHECK_RESULT(res);

            res = GetPeersBunch();
            CHECK_RESULT(res);
        }
        
        memcpy(&o_peer, &m_peers_bunch[m_current_peer++], sizeof(DataStructures::PeerInfo));

        if (m_current_peer >= m_peers_amount)
        {
            Results res = GetPeersBunch();
            if (res == Results::MaxPeersReached)
            {
                m_finished_peers = true;
                return Results::Success;
            }

            return res;
        }

        return Results::Success;
    }

    CommunicationManager::Results CommunicationManager::SendFilesList() const
    {
        FilesListMessage list_message;
        list_message.message_type = MessageTypes::FILES_LIST;

        Networks::Status result = m_server_socket.Send(&list_message, sizeof(list_message));
        if (result != Networks::Status::Success)
        {
            PRINT("Sending message %u failed with code %u\n", static_cast<uint32_t>(list_message.message_type), static_cast<uint32_t>(result));
            return Results::SendingFailed;
        }

        return Results::Success;
    }

    CommunicationManager::Results CommunicationManager::HandleFilesChunk(const FilesChunkMessage& files_chunk, bool print_desc)
    {
        if (m_current_fi_index != files_chunk.starting_index)
        {
            return SendFilesAck();
        }

        if (files_chunk.files_info_amount > MAX_FILES_INFO_IN_MESSAGE)
        {
            PRINT("got illegal value for files info amount %u\n", files_chunk.files_info_amount);
            return Results::BadFilesAmount;
        }

        for (size_t i = 0; i < files_chunk.files_info_amount; i++)
        {
            const DataStructures::FileInfo& cur_file = files_chunk.files_info[i];
            PRINT("%u\t\t%s\t", cur_file.file_id, cur_file.file_name);
            print_desc ? PRINT("%s\n", cur_file.file_desc) : PRINT("\n");
        }

        m_current_fi_index += files_chunk.files_info_amount;
        return SendFilesAck();
    }

    CommunicationManager::Results CommunicationManager::SendFilesAck() const
    {
        FilesAckMessage ack_message;
        ack_message.message_header.message_type = MessageTypes::FILES_ACK;
        ack_message.ack_index = m_current_fi_index;

        Networks::Status result = m_server_socket.Send(&ack_message, sizeof(ack_message));
        if (result != Networks::Status::Success)
        {
            PRINT("Sending message %u failed with code %u\n", static_cast<uint32_t>(ack_message.message_header.message_type), static_cast<uint32_t>(result));
            return Results::SendingFailed;
        }

        return Results::Success;
    }

    CommunicationManager::Results CommunicationManager::SendPeersList(const DataStructures::FileInfo& wanted_file) const
    {
        PeersListMessage list_message;
        list_message.message_header.message_type = MessageTypes::PEERS_LIST;
        memcpy(&list_message.wanted_file, &wanted_file, sizeof(list_message.wanted_file));

        Networks::Status result = m_server_socket.Send(&list_message, sizeof(list_message));
        if (result != Networks::Status::Success)
        {
            PRINT("Sending message %u failed with code %u\n", static_cast<uint32_t>(list_message.message_header.message_type), static_cast<uint32_t>(result));
            return Results::SendingFailed;
        }

        return Results::Success;
    }

    CommunicationManager::Results CommunicationManager::GetPeersBunch()
    {
        MaxServerMessage recv_msg;
        size_t recv_size = 0;
        Networks::Status recv_result = m_server_socket.Receive(&recv_msg, sizeof(recv_msg), recv_size);
        if (recv_result != Networks::Status::Success)
        {
            PRINT("Socket reading failed with error %u\n", static_cast<uint32_t>(recv_result));
            return Results::ReceivingFailed;
        }

        if (recv_msg.message_header.message_type == MessageTypes::PEERS_CHUNK)
        {
            if (recv_size < PeersChunkMessage::MINIMAL_SIZE)
            {
                PRINT("Tried to construct peers chunk message with %lu bytes\n", recv_size);
                return Results::ReceivedBadMessageSize;
            }

            Results res = HandlePeersChunk(reinterpret_cast<PeersChunkMessage&>(recv_msg));
            CHECK_RESULT(res);
        }
        else if (recv_msg.message_header.message_type == MessageTypes::PEERS_FIN)
        {
            return Results::MaxPeersReached;
        }
        else
        {
            PRINT("Got bad message type: %u\n", static_cast<uint32_t>(recv_msg.message_header.message_type));
            return Results::ReceivedBadMessageType;
        }

        return Results::Success;
    }

    CommunicationManager::Results CommunicationManager::HandlePeersChunk(const PeersChunkMessage& peers_chunk)
    {
        if (m_current_pi_index != peers_chunk.starting_index)
        {
            Results res = SendPeersAck();
            CHECK_RESULT(res);

            return GetPeersBunch();
        }

        if (peers_chunk.peers_info_amount > MAX_PEERS_INFO_IN_MESSAGE)
        {
            PRINT("got illegal value for peers info amount %u\n", peers_chunk.peers_info_amount);
            return Results::BadPeersAmount;
        }

        memcpy(m_peers_bunch, peers_chunk.peers_info, peers_chunk.peers_info_amount * sizeof(DataStructures::PeerInfo));
        m_peers_amount = peers_chunk.peers_info_amount;
        m_current_pi_index += peers_chunk.peers_info_amount;
        m_current_peer = 0;

        return SendPeersAck();
    }

    CommunicationManager::Results CommunicationManager::SendPeersAck() const
    {
        PeersAckMessage ack_message = {};
        ack_message.message_header.message_type = MessageTypes::PEERS_ACK;
        ack_message.ack_index = m_current_pi_index;

        Networks::Status result = m_server_socket.Send(&ack_message, sizeof(ack_message));
        if (result != Networks::Status::Success)
        {
            PRINT("Sending message %u failed with code %u\n", static_cast<uint32_t>(ack_message.message_header.message_type), static_cast<uint32_t>(result));
            return Results::ReceivingFailed;
        }

        return Results::Success;
    }
}
