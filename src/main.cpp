#include <stdio.h>
#include <stdlib.h>
#include <ServerProtocol/CommunicationManager.h>
#include <Networks/UDPSocket.h>

int main(int argc, char* argv[])
{
    Networks::UDPSocket sock;
    uint32_t dest_ip = 0x00000000;
    uint32_t dest_port = 9001;
    Networks::Status result = sock.Init(dest_ip, dest_port);
    if (result != Networks::Status::Success)
    {
        printf("socket init failed with code %u\n", static_cast<uint32_t>(result));
        return 1;
    }

    ServerProtocol::CommunicationManager com(sock);
    bool final = com.PrintAvailableFiles();
    printf(final ? "program finished successfully\n" : "program finished with failure\n");

    return 0;
}