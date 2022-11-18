#ifndef __NETWORKS_UDP_SOCKET_H__
#define __NETWORKS_UDP_SOCKET_h__

#if defined(WIN32)
#include <Networks/OSI/Windows/UDPSocket.h>
#elif defined(LINUX)
#include <Networks/OSI/Linux/UDPSocket.h>
#else
#error "Must define OS"
#endif

#endif //__NETWORKS_UDP_SOCKET_h__