#include "messages.h"
#include "user.h"
#include "network.h"
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

static struct host_info local_host;

static void make_msg(uint32 time, int command, const char *msg, char *buff, int *buff_len)
{
    int pack_len;
    
    pack_len = sprintf(buff, "%d:%u:%s:%s:%u", MSG_VERSION, time, local_host.user_name, local_host.host_name, command);
    
    if(buff_len)
        *buff_len = pack_len;
}

static int socket_init()
{
    if(!net_init())
        return 0;
    
    if(net_open_udp(2426) != NET_ERR_SUCCESS)
        return 0;
    
    struct net_address addr;
    addr.ip[0] = 0;
    addr.port = 2425;
    
    if(net_bind(net_get_udp(), &addr) != NET_ERR_SUCCESS)
        return 0;
    
    if(net_set_block(net_get_udp(), 0) != NET_ERR_SUCCESS)
        return 0;
    
    if(net_set_broadcast(net_get_udp(), 1) != NET_ERR_SUCCESS)
        return 0;
    
    return 1;
}


int msg_init()
{
//    struct hostent* hent;
    
    if(!socket_init())
        return 0;
    
    if(gethostname(local_host.host_name, sizeof(local_host.host_name)) != 0)
        return 0;
    
    if(getlogin_r(local_host.user_name, sizeof(local_host.user_name)) != 0)
        return 0;
    
//    hent = gethostbyname(local_host.host_name);
    
    
    return 1;
}

void notify_status(enum user_status status)
{
    char buff[1024];
    int buff_len;
    
    make_msg(1, status, "", buff, &buff_len);
    
    struct net_address addr;
    strcpy(addr.ip, "127.0.0.1");
    addr.port = 2425;
    
    net_send_to(&addr, buff, buff_len);
}