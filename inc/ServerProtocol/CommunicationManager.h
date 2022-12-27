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

        /**
         * Get all of the available files from the server and prints their info.
         * 
         * @param print_desc    whether the application should or shouldn't print the file description from the server
         * 
         * @return              true on success, false otherwise
         */
        bool PrintAvailableFiles(bool print_desc = false);

        /**
         * Get a peer which has the wanted file. Wanted file shouldn't change between calls.
         * 
         * @param wanted_file   the file for which we want a peer
         * @param o_peer        a peer which has the file and we haven't got yet
         * 
         * @return              true on success, false otherwise
         */
        bool GetPeerForFile(DataStructures::FileInfo& wanted_file, DataStructures::PeerInfo& o_peer);

    private:

        /**
         * Sends the FileList message to the server
         * 
         * @return              true on success, false otherwise
         */
        bool SendFilesList();

        /**
         * Handles the FileChunk message from the server
         * 
         * @param files_chunk   the message from the server
         * @param print_desc    same as PrintAvailableFiles
         * 
         * @return              true on success, false otherwise
         */
        bool HandleFilesChunk(FilesChunkMessage& files_chunk, bool print_desc = false);

        /**
         * Sends the FilesAck message to the server
         * 
         * @return              true on success, false otherwise
         */
        bool SendFilesAck();

        /**
         * Sends the PeersList message to the server, for the given file
         * 
         * @param wanted_file   the file for which we want a peer
         * 
         * @return              true on success, false otherwise
         */
        bool SendPeersList(DataStructures::FileInfo& wanted_file);

        /**
         * Tries to receive a PeersChunk from the server, and save to memory.
         * This function tries to refills the bunch if we take the last peer in the bunch.
         * 
         * @return              true on success or last peer, false otherwise
         */
        bool GetPeersBunch();
        
        /**
         * Handles the PeersChunk message
         * 
         * @return              true on success, false otherwise
         */
        bool HandlePeersChunk(PeersChunkMessage& peers_chunk);
        
        /**
         * Sends the PeersAck message to the server
         * 
         * @return              true on success, false otherwise
         */
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