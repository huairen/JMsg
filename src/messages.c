#include "messages.h"
#include "user.h"
#include "network.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <time.h>

#else
#include <unistd.h>
#include <netdb.h>

#define strcpy_s(dest,dest_len,src) strlcpy(dest,src,dest_len)
#define sprintf_s snprintf

#endif

extern char *separate_token(char *buf, char separetor, char **handle);
extern int local_to_unix(const char *src, char *dest, int dest_len);
extern int unix_to_local(const char *src, char *dest, int dest_len);

struct broadcast_list{
	char ip[MAX_IP_LEN];
	struct broadcast_list *next;
};

static struct broadcast_list broadcast_head;
static struct host_info local_host;

//IPMSG的报文格式：版本号:包编号:发送者姓名:发送者主机名:命令字:附加信息
static int make_msg(int command, const char *msg, char *buff, int *buff_len)
{
    int packet_len;
	int packet_id;
	int max_len;

	if(buff_len == 0)
		return 0;

	max_len = *buff_len;
	packet_id = (int)time(NULL) + 1;
    
    packet_len = sprintf_s(buff, max_len, "%d:%u:%s:%s:%u", MSG_VERSION, packet_id, local_host.user_name, local_host.host_name, command);
    
	if(msg) {
		packet_len += local_to_unix(msg, buff + packet_len, max_len - packet_len);
	}

    *buff_len = packet_len;
	return 1;
}

static int parse_msg(char *buff, int buff_len, struct msg_packet *pak)
{
	char *ex_str = NULL, *tok, *p;
	int len, ex_len = 0;

	len = (int)strlen(buff);

	if(buff_len > len + 1) {
		ex_str = buff + len + 1;
		ex_len = (int)strlen(ex_str);
	}

	if((tok = separate_token(buff, ':', &p)) == NULL)
		return 0;
	if((pak->version = atoi(tok)) != MSG_VERSION)
		return 0;

	if((tok = separate_token(NULL, ':', &p)) == NULL)
		return 0;
	pak->id = atoi(tok);

	if ((tok = separate_token(NULL, ':', &p)) == NULL)
		return 0;
	strcpy_s(pak->host.user_name, sizeof(pak->host.user_name), tok);

	if ((tok = separate_token(NULL, ':', &p)) == NULL)
		return 0;
	strcpy_s(pak->host.host_name, sizeof(pak->host.host_name), tok);

	if((tok = separate_token(NULL, ':', &p)) == NULL)
		return 0;

	pak->command = atoi(tok);
	pak->msg[0] = 0;
	pak->msg_ex[0] = 0;
	
	if((tok = separate_token(NULL, 0, &p)))
		unix_to_local(tok, pak->msg, sizeof(pak->msg));

	return 1;
}

static void make_broadcast_list()
{
	int broadcast_mode = cfg_broadcast_mode();
	struct broadcast_list *broad_ptr = NULL;

	broadcast_head.ip[0] = 0;

	if(broadcast_mode & DIRECT_BROADCAST) {
		struct net_device *device = net_device_list();
		struct net_device *dev_ptr = device;

		if(device != NULL)
			broad_ptr = &broadcast_head;

		while(dev_ptr) {
			if(broad_ptr != &broadcast_head) {
				struct broadcast_list *cmp_ptr = &broadcast_head;
				int is_exist = 0;
				while (cmp_ptr) {
					if(strcmp(cmp_ptr->ip, dev_ptr->broad) == 0) {
						is_exist = 1;
						break;
					}
					cmp_ptr = cmp_ptr->next;
				}	

				if(is_exist) {
					dev_ptr = dev_ptr->next;
					continue;;
				}
			}

			strcpy_s(broad_ptr->ip, sizeof(broad_ptr->ip), dev_ptr->broad);
			dev_ptr = dev_ptr->next;

			if(dev_ptr && broad_ptr->next == NULL) {
				broad_ptr->next = (struct broadcast_list*)malloc(sizeof(struct broadcast_list));
				memset(broad_ptr->next, 0, sizeof(struct broadcast_list));
			}

			broad_ptr = broad_ptr->next;
		}

		net_device_list_free(device);
	}

	if((broadcast_head.ip[0] == 0) || (broadcast_mode & LIMIT_BROADCAST)) {
		if(broadcast_head.ip[0] == 0)
			broad_ptr = &broadcast_head;

		if(broad_ptr == NULL) {
			broad_ptr = &broadcast_head;
			while (broad_ptr->next != NULL)
				broad_ptr = broad_ptr->next;

			broad_ptr->next = (struct broadcast_list *)malloc(sizeof(struct broadcast_list));
			memset(broad_ptr->next, 0, sizeof(struct broadcast_list));
		}
		
		if(broad_ptr) {
			strcpy_s(broad_ptr->ip, sizeof(broad_ptr->ip), "255.255.255.255");
			broad_ptr = broad_ptr->next;
		}
	}

	while(broad_ptr) {
		broad_ptr->ip[0] = 0;
		broad_ptr = broad_ptr->next;
	}
}

static int msg_send(const char* ip_addr, int command, const char *message)
{
	struct net_address addr;
	char buff[MAX_UDPBUF];
	int buff_len = MAX_UDPBUF;

	strcpy_s(addr.ip, sizeof(addr.ip), ip_addr);
	addr.port = cfg_bind_port();

	if(!make_msg(command, message, buff, &buff_len))
		return 0;

	return net_send_to(&addr, buff, buff_len);
}

/*
 * 对外接口
 */
int msg_init()
{
	char host[MAX_NAMELEN];
	struct hostent* hostinfo;

	if(!net_init())
		return 0;

	if(gethostname(host, sizeof(host)) == -1)
		host[0] = 0;
    
    //get hostname and username
#ifdef _WIN32
	{
        uint32 name_len = sizeof(local_host.host_name);
        if(!GetComputerName(local_host.host_name, &name_len))
            return 0;

        name_len = sizeof(local_host.user_name);
        if(!GetUserName(local_host.user_name, &name_len))
            return 0;
    }
#else
	if(host[0] == 0)
		return 0;

	strcpy_s(local_host.host_name, sizeof(local_host.host_name), host);
    if(getlogin_r(local_host.user_name, sizeof(local_host.user_name)) != 0)
        return 0;
#endif

	hostinfo = gethostbyname(host);
    if(hostinfo) {
		int device_index = cfg_net_device();
		if(device_index >= hostinfo->h_length-1)
			device_index = 0;

		inet_ntop(hostinfo->h_addrtype, hostinfo->h_addr_list[0], local_host.addr.ip, sizeof(local_host.addr.ip));
	}
	
	local_host.addr.port = cfg_bind_port();

	if(net_udp_open(cfg_bind_port(), 0) != NET_ERR_SUCCESS)
		return 0;

	if(net_bind(net_udp_socket(), &local_host.addr))
		return 0;

	memset(&broadcast_head, 0, sizeof(broadcast_head));
	make_broadcast_list();
    return 1;
}

int msg_recv(struct msg_packet *packet)
{
	struct net_address addr;
	char buff[MAX_UDPBUF];
	int buff_len;

	buff_len = net_recv_from(&addr, buff, MAX_UDPBUF);
	if(buff_len <= 0)
		return 0;

	packet->host.addr = addr;
	return parse_msg(buff, buff_len, packet);
}

void notify_status(enum user_status status)
{
}

int broadcast(int cmd, const char* msg)
{
	struct broadcast_list *broad_ptr = &broadcast_head;
	
	while(broad_ptr) {
		if(!msg_send(broad_ptr->ip, cmd, msg))
			return 0;
		broad_ptr = broad_ptr->next;
	}

	return 1;
}
