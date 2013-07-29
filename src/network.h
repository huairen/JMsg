#ifndef NETWORK_H_
#define NETWORK_H_

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#endif

enum net_error {
    NET_ERR_SUCCESS,
    NET_ERR_WOULD_BLOCK,
    NET_ERR_UNKNOWN,
};

struct net_address {
    char ip[20];
    unsigned short port;
};


int net_init();
void net_shutdown();

//udp
int net_open_udp(int port);
int net_get_udp();
void net_close_udp();
int net_send_to(struct net_address *addr, const char* buffer, int buff_len);

//
int net_bind(int socket, struct net_address *addr);
int net_set_broadcast(int socket, int broadcast);
int net_set_block(int socket, int block);

void net_process();


#endif
