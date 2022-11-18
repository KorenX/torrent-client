#include <stdio.h>
#include <stdlib.h>
#include <ServerProtocol/Messages.h>

int main(int argc, char* argv[])
{
    ServerProtocol::ThanksMessage thanks;
    thanks.message_type = ServerProtocol::MessageTypes::THANKS;
    printf("%d\n", static_cast<uint8_t>(thanks.message_type));
    return 0;
}