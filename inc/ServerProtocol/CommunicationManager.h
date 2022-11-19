#ifndef __SERVER_PROTOCOL_COMMUNICATION_MANAGER_H__
#define __SERVER_PROTOCOL_COMMUNICATION_MANAGER_H__

#include <Networks/UDPSocket.h>
#include <ServerProtocol/Messages.h>

namespace ServerProtocol
{
    class CommunicationManager
    {
    public:
        CommunicationManager(Networks::UDPSocket& server_socket) : m_server_socket(server_socket), m_current_fi_index(0) {}

        bool PrintAvailableFiles(bool print_desc = false);

        bool GetPeerForFile(DataStructures::FileInfo& wanted_file, DataStructures::PeerInfo& o_peer);

    private:

        bool SendFilesList();

        bool HandleFilesChunk(FilesChunkMessage& files_chunk, bool print_desc = false);

        bool SendFilesAck();

        Networks::UDPSocket& m_server_socket;
        uint32_t m_current_fi_index;
    };
}

#endif //__SERVER_PROTOCOL_COMMUNICATION_MANAGER_H__