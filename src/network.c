#include "network.h"
#include <stdio.h>
#include <memory.h>

#ifdef _WIN32
#include <WinSock2.h>

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define closesocket close

#endif

static int init_count = 0;

static int udp_socket = INVALID_SOCKET;
static int udp_port = 0;

#define MAX_PACKET_SIZE 1024

static void net_to_socket_address(const struct net_address *addr, struct sockaddr_in *sock_addr)
{
    memset(sock_addr, 0, sizeof(struct sockaddr_in));
    sock_addr->sin_family = AF_INET;
    sock_addr->sin_port = htons(addr->port);
    if(addr->ip[0] == 0)
        sock_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    else
        sock_addr->sin_addr.s_addr = inet_addr(addr->ip);
}

static void socket_to_net_address(const struct sockaddr_in *sock_addr, struct net_address *addr)
{
    strcpy(addr->ip, inet_ntoa(sock_addr->sin_addr));
    addr->port = ntohs(sock_addr->sin_port);
}

static int get_last_error()
{
#ifdef _WIN32
    int err = WSAGetLastError();
    switch (err) {
        case 0:
            return NET_ERR_SUCCESS;
        case WSAEWOULDBLOCK:
            return NET_ERR_WOULD_BLOCK;
    }
#else
    if(errno == EAGAIN)
        return NET_ERR_WOULD_BLOCK;
    else if(errno == 0)
        return NET_ERR_SUCCESS;
#endif
    return NET_ERR_UNKNOWN;
}

int net_init()
{
#ifdef _WINDOWS
    if(!init_count)
    {
        
    }
#endif
    
    init_count++;
    return 1;
}

void net_shutdown()
{
    net_close_udp();
    init_count--;
    
#ifdef _WINDOWS
    if(!init_count)
    {
        
    }
#endif
}

int net_open_udp(int port)
{
    if(udp_socket != INVALID_SOCKET)
        net_close_udp();
    
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket == INVALID_SOCKET)
        return get_last_error();
    
    udp_port = port;
    return NET_ERR_SUCCESS;
}

int net_get_udp()
{
    return udp_socket;
}

void net_close_udp()
{
    if (udp_socket != INVALID_SOCKET) {
		closesocket(udp_socket);
        udp_socket = INVALID_SOCKET;
    }
}

int net_send_to(struct net_address *addr, const char* buffer, int buff_len)
{
    struct sockaddr_in sock_addr;
    net_to_socket_address(addr, &sock_addr);
    if(sendto(udp_socket, buffer, buff_len, 0,
              (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR)
        return get_last_error();
    return NET_ERR_SUCCESS;
}

int net_bind(int socket, struct net_address *addr)
{
    int err;
    struct sockaddr_in sock_addr;
    
    net_to_socket_address(addr, &sock_addr);
    err = bind(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (err == 0)
        return NET_ERR_SUCCESS;
    return get_last_error();
}

int net_set_broadcast(int socket, int broadcast)
{
    int err = setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));
    if(err == 0)
        return NET_ERR_SUCCESS;
    return get_last_error();
}

int net_set_block(int socket, int block)
{
    int notblock = !block;
#ifdef _WIN32
    int err = ioctlsocket(socket, FIONBIO, &notblock);
#else
    int err = fcntl(socket, F_SETFL, O_NONBLOCK, notblock);
#endif
    if(err == 0)
        return NET_ERR_SUCCESS;
    return get_last_error();
}

void net_process()
{
    ssize_t bytes_read = -1;
    char temp_buff[MAX_PACKET_SIZE];
    struct sockaddr sa;
    socklen_t addr_len = sizeof(sa);
    
    if(udp_socket != INVALID_SOCKET)
        bytes_read = recvfrom(udp_socket, temp_buff, MAX_PACKET_SIZE, 0, &sa, &addr_len);
}