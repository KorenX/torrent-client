#include <ServerProtocol/CommunicationManager.h>
#include <stdio.h>

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
            printf("Socket reading failed with error %u\n", static_cast<uint32_t>(recv_result));
            return false;
        }

        
        printf("FILE ID\t\tFILE NAME");
        print_desc ? printf("\tFILE DESC\n") : printf("\n");
        while (recv_msg.message_header.message_type == MessageTypes::FILES_CHUNK)
        {
            if (recv_size < FilesChunkMessage::MINIMAL_SIZE)
            {
                printf("Tried to construct files chunk message with %lu bytes\n", recv_size);
                return false;
            }

            bool success = HandleFilesChunk(reinterpret_cast<FilesChunkMessage&>(recv_msg), print_desc);
            if (!success)
            {
                printf("Handle Files Chunk Failed\n");
                return false;
            }

            recv_result = m_server_socket.Receive(&recv_msg, sizeof(recv_msg), recv_size);
            if (recv_result != Networks::Status::Success)
            {
                printf("Socket reading failed with error %u\n", static_cast<uint32_t>(recv_result));
                return false;
            }
        }

        printf("-------------------------\n");

        return recv_msg.message_header.message_type == MessageTypes::FILES_FIN;
    }

    bool CommunicationManager::GetPeerForFile(DataStructures::FileInfo& wanted_file, DataStructures::PeerInfo& o_peer)
    {
        return false;
    }

    bool CommunicationManager::SendFilesList()
    {
        FilesListMessage list_message;
        list_message.message_type = MessageTypes::FILES_LIST;

        Networks::Status result = m_server_socket.Send(&list_message, sizeof(list_message));
        if (result != Networks::Status::Success)
        {
            printf("Sending message %u failed with code %u\n", static_cast<uint32_t>(list_message.message_type), static_cast<uint32_t>(result));
            return false;
        }

        return true;
    }

    bool CommunicationManager::HandleFilesChunk(FilesChunkMessage& files_chunk, bool print_desc)
    {
        if (m_current_fi_index != files_chunk.starting_index)
        {
            return SendFilesAck();
        }

        if (files_chunk.files_info_amount > MAX_FILES_INFO_IN_MESSAGE)
        {
            printf("got illegal value for files info amount %u\n", files_chunk.files_info_amount);
            return false;
        }

        for (size_t i = 0; i < files_chunk.files_info_amount; i++)
        {
            DataStructures::FileInfo& cur_file = files_chunk.files_info[i];
            printf("%u\t\t%s\t", cur_file.file_id, cur_file.file_name);
            print_desc ? printf("%s\n", cur_file.file_desc) : printf("\n");
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
            printf("Sending message %u failed with code %u\n", static_cast<uint32_t>(ack_message.message_header.message_type), static_cast<uint32_t>(result));
            return false;
        }

        return true;
    }
}
