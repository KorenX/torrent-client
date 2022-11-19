#ifndef __NETWORKS_UDP_SOCKET_H__
#define __NETWORKS_UDP_SOCKET_H__

#include <stdint.h>
#include <Networks/Status.h>
#include <sys/socket.h>
#include <netdb.h>

namespace Networks
{
    class UDPSocket
    {
    public:

        UDPSocket() : m_init(false), m_socket_fd(0), m_dest({}) {}

        Status Init(const uint32_t dest_ip, const uint16_t dest_port);

        Status Send(const void* buffer, const size_t buffer_size);

        Status Receive(void* o_buffer, const size_t buffer_size, size_t& o_read);
        
    private:
        bool m_init;
        int m_socket_fd;
        // addrinfo m_dest;
        sockaddr_in m_dest;
    };
}

#endif //__NETWORKS_UDP_SOCKET_H__
