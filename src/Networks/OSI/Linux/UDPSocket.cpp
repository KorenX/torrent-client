#include <Networks/UDPSocket.h>

namespace Networks
{
    Status UDPSocket::Init(const uint32_t dest_ip, const uint16_t dest_port)
    {
        m_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket_fd < 0)
        {
            return Networks::Status::SocketCreationFailed;
        }

        m_dest.sin_family = AF_INET;
        m_dest.sin_addr.s_addr = htonl(dest_ip);
        m_dest.sin_port = htons(dest_port);
        
        m_init = true;
        return Status::Success;
    }

    Status UDPSocket::Send(const void* buffer, const size_t buffer_size)
    {
        if (!m_init)
        {
            return Networks::Status::NotInitialized;
        }

        if (buffer == nullptr)
        {
            return Networks::Status::InvalidNullptr;
        }

        int written = sendto(m_socket_fd, reinterpret_cast<const char*>(buffer), buffer_size, 0, reinterpret_cast<sockaddr*>(&m_dest), sizeof(m_dest));
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

    Status UDPSocket::Receive(void* o_buffer, const size_t buffer_size, size_t& o_read)
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
        int read = recvfrom(m_socket_fd, reinterpret_cast<char*>(o_buffer), buffer_size, 0, reinterpret_cast<sockaddr*>(&m_dest), reinterpret_cast<socklen_t*>(&dest_len));
        if (read < 0)
        {
            return Networks::Status::ReadFailed;
        }

        o_read = read;

        return Networks::Status::Success;
    }
}