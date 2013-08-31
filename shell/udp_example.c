#include <stdio.h>
#include <string.h>
#include "network.hxx"

void udp_client()
{
    struct net_address addr;
    char buff[512];
    int bytes_read;
    
	net_init();
    net_udp_open(0, 0);
    
    strcpy_s(addr.ip, sizeof(addr.ip), "127.0.0.1");
    addr.port = 13451;
    
    bytes_read = net_send_to(&addr, "test udp", strlen("test udp"));
    if(bytes_read > 0) {
        printf("send ok!\n");
        bytes_read = net_recv_from(&addr, buff, sizeof(buff));
        if(bytes_read > 0) {
            buff[bytes_read] = 0;
            printf("recv: %s\n", buff);
        }
    }

	net_shutdown();
}

void udp_server()
{
    struct net_address addr;
    char buff[512];
    int bytes_read;

	net_init();
    net_udp_open(13451, 1);
    
    while (1) {
        bytes_read = net_recv_from(&addr, buff, sizeof(buff));
        if(bytes_read > 0) {
            buff[bytes_read] = 0;
            printf("%s %d says: %s\n", addr.ip, addr.port, buff);
            
            net_send_to(&addr, buff, bytes_read);
        }
        printf("next...\n");
    }

	net_shutdown();
}