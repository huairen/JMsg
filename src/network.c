#include "network.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <IPHlpApi.h>
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
    strcpy_s(addr->ip, sizeof(addr->ip), inet_ntoa(sock_addr->sin_addr));
    addr->port = ntohs(sock_addr->sin_port);
}

static void calc_broadcast_addr(const char *ip, const char *mask, char *broadcast, int len)
{
	int ip_addr = inet_addr(ip);
	int mask_addr = inet_addr(mask);
	int broad_addr = (ip_addr & mask_addr) | ~mask_addr;
	strcpy_s(broadcast, len, inet_ntoa(*(struct in_addr*)&broad_addr));
}

static int get_last_error()
{
#ifdef _WIN32
	int err = WSAGetLastError();
	printf("error: %d\n", err);
    switch (err) {
        case 0:
            return NET_ERR_SUCCESS;
        case WSAEWOULDBLOCK:
            return NET_ERR_WOULD_BLOCK;
    }
#else
    printf("error: %d\n", errno);
    if(errno == EAGAIN)
        return NET_ERR_WOULD_BLOCK;
    else if(errno == 0)
        return NET_ERR_SUCCESS;
#endif
    return NET_ERR_UNKNOWN;
}


//---------------------------------------------------------------

int net_init()
{
#ifdef _WIN32
    if(!init_count)
    {
		WSADATA wsd;
        if(WSAStartup(MAKEWORD(2,2), &wsd) != 0 &&
			WSAStartup(MAKEWORD(1,1), &wsd) != 0)
			return 0;
    }
#endif
    
    init_count++;
    return 1;
}

void net_shutdown()
{
    net_udp_close();
    init_count--;
    
#ifdef _WIN32
    if(!init_count)
    {
        WSACleanup();
    }
#endif
}

int net_udp_open(int port, int is_bind)
{
    if(udp_socket != INVALID_SOCKET)
        net_udp_close();
    
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket == INVALID_SOCKET)
        return get_last_error();
    
    if(is_bind) {
        int err = net_bind_port(udp_socket, port);
        if(err != NET_ERR_SUCCESS) {
            net_udp_close();
            return err;
        }
    }
    
    udp_port = port;
    return NET_ERR_SUCCESS;
}

void net_udp_close()
{
    if (udp_socket != INVALID_SOCKET) {
		closesocket(udp_socket);
        udp_socket = INVALID_SOCKET;
    }
}

int net_udp_socket()
{
	return udp_socket;
}

int net_send_to(struct net_address *addr, const char* buffer, int buff_len)
{
    int bytes_send;
    struct sockaddr_in sock_addr;
    net_to_socket_address(addr, &sock_addr);
    
    bytes_send = (int)sendto(udp_socket, buffer, buff_len, 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    return bytes_send;
}

int net_recv_from(struct net_address *addr, char* buffer, int buff_len)
{
    int bytes_read = -1;
    struct sockaddr_in sock_addr;
    unsigned int addr_len = sizeof(sock_addr);
    
    if(udp_socket != INVALID_SOCKET)
        bytes_read = (int)recvfrom(udp_socket, buffer, buff_len, 0, (struct sockaddr*)&sock_addr, &addr_len);
    
    if(bytes_read > 0)
        socket_to_net_address(&sock_addr, addr);
    
    return bytes_read;
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

int net_bind_port(int socket, int port)
{
    int err;
    struct sockaddr_in sock_addr;
    
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    err = bind(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if(err == 0)
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

#ifdef _WIN32
struct net_device* net_device_list()
{
	struct net_device *device = NULL;
	struct net_device **device_ptr = &device;

	unsigned long buff_size;
	PIP_ADAPTER_INFO adapter_info;
	
	GetAdaptersInfo(NULL, &buff_size);

	adapter_info = (PIP_ADAPTER_INFO)malloc(buff_size);
	if(GetAdaptersInfo(adapter_info, &buff_size) == ERROR_SUCCESS) {
		int mac_len = sizeof(device->mac);

		while(adapter_info)	{
			unsigned int i;
			int buff_pos = 0;
			IP_ADDR_STRING *addr_str = &adapter_info->IpAddressList;
			struct net_device *new_dev = (struct net_device *)malloc(sizeof(struct net_device));
			
			if(new_dev == NULL)	{
				net_device_list_free(device);
				return NULL;
			}

			strcpy_s(new_dev->name, sizeof(new_dev->name), adapter_info->Description);
			
			for (i=0; i<adapter_info->AddressLength; ++i) {
				sprintf_s(new_dev->mac + buff_pos, mac_len - buff_pos,
					i ? "-%02x" : "%02x", adapter_info->Address[i]);

				buff_pos = strlen(new_dev->mac);
			}
			
			while(addr_str) {
				strcpy_s(new_dev->ip, sizeof(new_dev->ip), addr_str->IpAddress.String);
				strcpy_s(new_dev->mask, sizeof(new_dev->mask), addr_str->IpMask.String);
				calc_broadcast_addr(new_dev->ip,new_dev->mask,new_dev->broad, sizeof(new_dev->broad));
				addr_str = addr_str->Next;
			}

			new_dev->next = NULL;
			*device_ptr = new_dev;
			device_ptr = &new_dev->next;

			adapter_info = adapter_info->Next;
		}
	}

	return device;
}

#else
struct net_device* net_device_list()
{
	pcap_findalldevs;
	ioctl (fd, SIOCGIFCONF, (char *) &ifc);
	return 0;
}

#endif

void net_device_list_free(struct net_device *list)
{
	if(list != NULL) {
		struct net_device *tmp = list;
		list = list->next;
		free(tmp);
	}
}