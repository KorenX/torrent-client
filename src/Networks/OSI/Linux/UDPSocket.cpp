#include <Networks/UDPSocket.h>

namespace Networks
{
    Status UDPSocket::Init(const uint32_t dest_ip, const uint16_t dest_port)
    {
        m_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket_fd < 0)
        {
            return Status::SocketCreationFailed;
        }

        m_dest.sin_family = AF_INET;
        m_dest.sin_addr.s_addr = htonl(dest_ip);
        m_dest.sin_port = htons(dest_port);
        
        m_init = true;
        return Status::Success;
    }

    Status UDPSocket::Send(const void* buffer, const size_t buffer_size) const
    {
        if (!m_init)
        {
            return Status::NotInitialized;
        }

        if (buffer == nullptr)
        {
            return Status::InvalidNullptr;
        }

        ssize_t written = sendto(m_socket_fd, buffer, buffer_size, 0, reinterpret_cast<const sockaddr*>(&m_dest), sizeof(m_dest));
        if (written < 0)
        {
            return Status::WriteFailed;
        }

        if (static_cast<size_t>(written) != buffer_size)
        {
            return Status::PartiallyWritten;
        }

        return Status::Success;
    }

    Status UDPSocket::Receive(void* o_buffer, const size_t buffer_size, size_t& o_read)
    {
        return ReceiveTimeout(o_buffer, buffer_size, o_read, 0);
    }

    Status UDPSocket::ReceiveTimeout(void* o_buffer, const size_t buffer_size, size_t& o_read, size_t timeout_ms)
    {
        if (!m_init)
        {
            return Status::Success;
        }

        if (o_buffer == nullptr)
        {
            return Status::InvalidNullptr;
        }

        // can add optimization of saving the current timeout to save the syscall
        struct timeval timeout;
        timeout.tv_sec = timeout_ms / MILISECONDS_IN_SECONDS;
        timeout.tv_usec = timeout_ms % MILISECONDS_IN_SECONDS;
        if (setsockopt(m_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
        {
            return Status::SetTimeoutFailed;
        }

        int dest_len = sizeof(m_dest);
        ssize_t read = recvfrom(m_socket_fd, o_buffer, buffer_size, 0, reinterpret_cast<sockaddr*>(&m_dest), reinterpret_cast<socklen_t*>(&dest_len));
        if (read < 0)
        {
            return Status::ReadFailed;
        }

        o_read = read;

        return Status::Success;
    }
}