#ifndef __SERVER_PROTOCOL_COMMUNICATION_MANAGER_H__
#define __SERVER_PROTOCOL_COMMUNICATION_MANAGER_H__

#include <Networks/UDPSocket.h>
#include <ServerProtocol/Messages.h>

namespace ServerProtocol
{
    class CommunicationManager
    {
    public:
        CommunicationManager(Networks::UDPSocket& server_socket) :  m_server_socket(server_socket), 
                                                                    m_current_fi_index(0),
                                                                    m_current_pi_index(0),
                                                                    m_peers_bunch({}),
                                                                    m_current_peer(0),
                                                                    m_peers_amount(0) {}

        bool PrintAvailableFiles(bool print_desc = false);

        bool GetPeerForFile(DataStructures::FileInfo& wanted_file, DataStructures::PeerInfo& o_peer);

    private:

        bool SendFilesList();

        bool HandleFilesChunk(FilesChunkMessage& files_chunk, bool print_desc = false);

        bool SendFilesAck();

        bool SendPeersList(DataStructures::FileInfo& wanted_file);

        bool GetPeersBunch();

        bool HandlePeersChunk(PeersChunkMessage& peers_chunk);
        
        bool SendPeersAck();
        
        Networks::UDPSocket& m_server_socket;
        uint32_t m_current_fi_index;
        uint32_t m_current_pi_index;
        DataStructures::PeerInfo m_peers_bunch[MAX_PEERS_INFO_IN_MESSAGE];
        uint32_t m_current_peer;
        uint32_t m_peers_amount;
    };
}

#endif //__SERVER_PROTOCOL_COMMUNICATION_MANAGER_H__