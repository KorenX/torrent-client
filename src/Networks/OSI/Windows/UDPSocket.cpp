#include <Networks/UDPSocket.h>

Networks::Status Networks::UDPSocket::Init(const uint32_t dest_ip, const uint16_t dest_port)
{
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        return Networks::Status::SocketCreationFailed;
    }

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == SOCKET_ERROR)
    {
        return Networks::Status::SocketCreationFailed;
    }

    memset(&m_dest, 0, sizeof(m_dest));
    m_dest.sin_family = AF_INET;
    m_dest.sin_addr.S_un.S_addr = htonl(dest_ip);
    m_dest.sin_port = htons(dest_port);

    m_init = true;
    return Networks::Status::Success;
}

Networks::Status Networks::UDPSocket::Send(const void* buffer, const size_t buffer_size)
{
    if (!m_init)
    {
        return Networks::Status::NotInitialized;
    }

    if (buffer == nullptr)
    {
        return Networks::Status::InvalidNullptr;
    }

    int written = sendto(m_socket, reinterpret_cast<const char*>(buffer), buffer_size, 0, reinterpret_cast<sockaddr*>(&m_dest), sizeof(m_dest));
    if (written < 0)
    {
        return Networks::Status::WriteFailed;
    }

    if (static_cast<size_t>(written) != buffer_size)
    {
        return Networks::Status::PartiallyWritten;
    }

    return Networks::Status::Success;
}

Networks::Status Networks::UDPSocket::Receive(void* o_buffer, const size_t buffer_size, size_t& o_read)
{
    if (!m_init)
    {
        return Networks::Status::Success;
    }

    if (o_buffer == nullptr)
    {
        return Networks::Status::InvalidNullptr;
    }

    int dest_len = sizeof(m_dest);
    int read = recvfrom(m_socket, reinterpret_cast<char*>(o_buffer), buffer_size, 0, reinterpret_cast<sockaddr*>(&m_dest), &dest_len);
    if (read < 0)
    {
        return Networks::Status::ReadFailed;
    }

    o_read = read;

    return Networks::Status::Success;
}

