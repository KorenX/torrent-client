#include <stdio.h>
#include <stdlib.h>
#include <ServerProtocol/Messages.h>
#include <Networks/UDPSocket.h>

int main(int argc, char* argv[])
{
    ServerProtocol::ThanksMessage thanks;
    thanks.message_type = ServerProtocol::MessageTypes::THANKS;
    Networks::UDPSocket sock;
    uint32_t self_ip = 0x7f000001;
    uint32_t dest_port = 8989;
    sock.Init(self_ip, dest_port);
    printf("%d\n", static_cast<uint8_t>(thanks.message_type));
    return 0;
}