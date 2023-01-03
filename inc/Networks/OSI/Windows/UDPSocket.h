#ifndef __NETWORKS_UDP_SOCKET_H__
#define __NETWORKS_UDP_SOCKET_H__

#include <winsock2.h>
#include <stdint.h>
#include <Networks/Status.h>

namespace Networks
{
    class UDPSocket
    {
    public:

        UDPSocket() : m_init(false) {}

        /**
         * Initializes a socket for sending and receiving. The source port is randomized by OS.
         * 
         * @param dest_ip       destination ip for the socket connection
         * @param dest_port     destination port for the socket connection
         * 
         * @return              an appropriate return status
         */
        Status Init(const uint32_t dest_ip, const uint16_t dest_port);

        /**
         * Sends a buffer to the destination connection.
         * 
         * @param buffer        the buffer of data to send
         * @param buffer_size   the amount of bytes to send from the buffer
         * 
         * @return              an appropriate return status
         */
        Status Send(const void* buffer, const size_t buffer_size) const;

        /**
         * Receives a buffer from the destination connection.
         * 
         * @param o_buffer      the buffer of data to write into
         * @param buffer_size   the amount of bytes to try and receive
         * @param o_read        the amount of bytes actually written.
         * 
         * @return              an appropriate return status
         */
        Status Receive(void* o_buffer, const size_t buffer_size, size_t& o_read);

        /**
         * Receives a buffer from the destination connection, waiting a maximum of timeout_ms miliseconds.
         * 
         * @param o_buffer      the buffer of data to write into
         * @param buffer_size   the amount of bytes to try and receive
         * @param o_read        the amount of bytes actually written.
         * @param timeout_ms    the limit amount of miliseconds to wait.
         * 
         * @return              an appropriate return status
         */
        Status ReceiveTimeout(void* o_buffer, const size_t buffer_size, size_t& o_read, size_t timeout_ms);

    private:
        bool m_init;
        SOCKET m_socket;
        sockaddr_in m_dest;
    };
}

#endif //__NETWORKS_UDP_SOCKET_H__
