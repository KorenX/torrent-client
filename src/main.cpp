#include <stdio.h>
#include <stdlib.h>
#include <ServerProtocol/CommunicationManager.h>
#include <Networks/UDPSocket.h>

int main(int argc, char* argv[])
{
    Networks::UDPSocket sock;
    uint32_t dest_ip = 0x7f000001;
    uint16_t dest_port = 9001;
    Networks::Status result = sock.Init(dest_ip, dest_port);
    if (result != Networks::Status::Success)
    {
        printf("socket init failed with code %u\n", static_cast<uint32_t>(result));
        return 1;
    }

    ServerProtocol::CommunicationManager com(sock);
    if (!com.PrintAvailableFiles())
    {
        printf("File list failed\n");
        return 1;
    }
    printf("Enter the file id you wish to download:\n");
    DataStructures::FileInfo wanted_file = {};
    scanf("%u", &wanted_file.file_id);

    DataStructures::PeerInfo available_peer = {};
    if (!com.GetPeerForFile(wanted_file, available_peer))
    {
        printf("File list failed\n");
        return 1;
    }

    printf("gracefull exit");
    return 0;
}