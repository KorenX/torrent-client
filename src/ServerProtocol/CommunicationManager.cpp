#include <ServerProtocol/CommunicationManager.h>
#include <stdio.h>
#include <string.h>
#include <General/Log.h>

namespace ServerProtocol
{
    bool CommunicationManager::PrintAvailableFiles(bool print_desc)
    {
        if (!SendFilesList())
        {
            return false;
        }

        MaxServerMessage recv_msg;
        size_t recv_size = 0;
        Networks::Status recv_result = m_server_socket.Receive(&recv_msg, sizeof(recv_msg), recv_size);
        if (recv_result != Networks::Status::Success)
        {
            PRINT("Socket reading failed with error %u\n", static_cast<uint32_t>(recv_result));
            return false;
        }

        PRINT("FILE ID\t\tFILE NAME");
        print_desc ? PRINT("\tFILE DESC\n") : PRINT("\n");
        while (recv_msg.message_header.message_type == MessageTypes::FILES_CHUNK)
        {
            if (recv_size < FilesChunkMessage::MINIMAL_SIZE)
            {
                PRINT("Tried to construct files chunk message with %lu bytes\n", recv_size);
                return false;
            }

            bool success = HandleFilesChunk(reinterpret_cast<FilesChunkMessage&>(recv_msg), print_desc);
            if (!success)
            {
                PRINT("Handle Files Chunk Failed\n");
                return false;
            }

            recv_result = m_server_socket.Receive(&recv_msg, sizeof(recv_msg), recv_size);
            if (recv_result != Networks::Status::Success)
            {
                PRINT("Socket reading failed with error %u\n", static_cast<uint32_t>(recv_result));
                return false;
            }
        }

        PRINT("-------------------------\n");

        return recv_msg.message_header.message_type == MessageTypes::FILES_FIN;
    }

    bool CommunicationManager::GetPeerForFile(const DataStructures::FileInfo& wanted_file, DataStructures::PeerInfo& o_peer)
    {
        if (m_current_pi_index == 0)
        {
            if (!SendPeersList(wanted_file))
            {
                return false;
            }

            if (!GetPeersBunch())
            {
                return false;
            }
        }
        

        memcpy(&o_peer, &m_peers_bunch[m_current_peer++], sizeof(DataStructures::PeerInfo));

        if (m_current_peer >= m_peers_amount)
        {
            return GetPeersBunch();
        }

        return true;
    }

    bool CommunicationManager::SendFilesList()
    {
        FilesListMessage list_message;
        list_message.message_type = MessageTypes::FILES_LIST;

        Networks::Status result = m_server_socket.Send(&list_message, sizeof(list_message));
        if (result != Networks::Status::Success)
        {
            PRINT("Sending message %u failed with code %u\n", static_cast<uint32_t>(list_message.message_type), static_cast<uint32_t>(result));
            return false;
        }

        return true;
    }

    bool CommunicationManager::HandleFilesChunk(const FilesChunkMessage& files_chunk, bool print_desc)
    {
        if (m_current_fi_index != files_chunk.starting_index)
        {
            return SendFilesAck();
        }

        if (files_chunk.files_info_amount > MAX_FILES_INFO_IN_MESSAGE)
        {
            PRINT("got illegal value for files info amount %u\n", files_chunk.files_info_amount);
            return false;
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

    bool CommunicationManager::SendFilesAck()
    {
        FilesAckMessage ack_message;
        ack_message.message_header.message_type = MessageTypes::FILES_ACK;
        ack_message.ack_index = m_current_fi_index;

        Networks::Status result = m_server_socket.Send(&ack_message, sizeof(ack_message));
        if (result != Networks::Status::Success)
        {
            PRINT("Sending message %u failed with code %u\n", static_cast<uint32_t>(ack_message.message_header.message_type), static_cast<uint32_t>(result));
            return false;
        }

        return true;
    }

    bool CommunicationManager::SendPeersList(const DataStructures::FileInfo& wanted_file)
    {
        PeersListMessage list_message;
        list_message.message_header.message_type = MessageTypes::PEERS_LIST;
        memcpy(&list_message.wanted_file, &wanted_file, sizeof(list_message.wanted_file));

        Networks::Status result = m_server_socket.Send(&list_message, sizeof(list_message));
        if (result != Networks::Status::Success)
        {
            PRINT("Sending message %u failed with code %u\n", static_cast<uint32_t>(list_message.message_header.message_type), static_cast<uint32_t>(result));
            return false;
        }

        return true;
    }

    bool CommunicationManager::GetPeersBunch()
    {
        MaxServerMessage recv_msg;
        size_t recv_size = 0;
        Networks::Status recv_result = m_server_socket.Receive(&recv_msg, sizeof(recv_msg), recv_size);
        if (recv_result != Networks::Status::Success)
        {
            PRINT("Socket reading failed with error %u\n", static_cast<uint32_t>(recv_result));
            return false;
        }

        if (recv_msg.message_header.message_type == MessageTypes::PEERS_CHUNK)
        {
            if (recv_size < PeersChunkMessage::MINIMAL_SIZE)
            {
                PRINT("Tried to construct peers chunk message with %lu bytes\n", recv_size);
                return false;
            }

            bool success = HandlePeersChunk(reinterpret_cast<PeersChunkMessage&>(recv_msg));
            if (!success)
            {
                PRINT("Handle Peers Chunk Failed\n");
                return false;
            }
        }
        else if (recv_msg.message_header.message_type == MessageTypes::PEERS_FIN)
        {
            return true;
        }
        else
        {
            PRINT("Got bad message type: %u\n", static_cast<uint32_t>(recv_msg.message_header.message_type));
            return false;
        }

        return true;
    }

    bool CommunicationManager::HandlePeersChunk(const PeersChunkMessage& peers_chunk)
    {
        if (m_current_pi_index != peers_chunk.starting_index)
        {
            if (!SendPeersAck())
            {
                return false;
            }

            return GetPeersBunch();
        }

        if (peers_chunk.peers_info_amount > MAX_PEERS_INFO_IN_MESSAGE)
        {
            PRINT("got illegal value for peers info amount %u\n", peers_chunk.peers_info_amount);
            return false;
        }

        memcpy(m_peers_bunch, peers_chunk.peers_info, peers_chunk.peers_info_amount * sizeof(DataStructures::PeerInfo));
        m_peers_amount = peers_chunk.peers_info_amount;
        m_current_pi_index += peers_chunk.peers_info_amount;
        m_current_peer = 0;

        return SendPeersAck();
    }

    bool CommunicationManager::SendPeersAck()
    {
        PeersAckMessage ack_message = {};
        ack_message.message_header.message_type = MessageTypes::PEERS_ACK;
        ack_message.ack_index = m_current_pi_index;

        Networks::Status result = m_server_socket.Send(&ack_message, sizeof(ack_message));
        if (result != Networks::Status::Success)
        {
            PRINT("Sending message %u failed with code %u\n", static_cast<uint32_t>(ack_message.message_header.message_type), static_cast<uint32_t>(result));
            return false;
        }

        return true;
    }
}
