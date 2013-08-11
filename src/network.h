#ifndef NETWORK_H_
#define NETWORK_H_

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")
#endif

#define MAX_IP_LEN 20

enum net_error {
    NET_ERR_SUCCESS,
    NET_ERR_WOULD_BLOCK,
    NET_ERR_UNKNOWN,
};

struct net_address {
	char ip[MAX_IP_LEN];
	unsigned short port;
};

struct net_device {
	char name[100];
	char mac[100];
	char ip[MAX_IP_LEN];
	char mask[MAX_IP_LEN];
	char broad[MAX_IP_LEN];
	struct net_device* next;
};


int net_init();
void net_shutdown();

//udp
int net_udp_open(int port, int is_bind);
void net_udp_close();
int net_udp_socket();
int net_send_to(struct net_address *addr, const char* buffer, int buff_len);
int net_recv_from(struct net_address *addr, char* buffer, int buff_len);

//
int net_bind(int socket, struct net_address *addr);
int net_bind_port(int socket, int port);

int net_set_broadcast(int socket, int broadcast);
int net_set_block(int socket, int block);

//
struct net_device* net_device_list();
void net_device_list_free(struct net_device *list);


#endif
