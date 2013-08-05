#include "messages.h"
#include "network.h"
#include "config.h"

extern int make_msg(int command, const char *msg, const char *msg_ex, char *buff, int *buff_len);
extern int parse_msg(char *buff, int buff_len, struct msg_packet *packet);
extern struct cmd_handle cmd_table[];
extern const int cmd_table_count;

struct broadcast_list{
	char ip[MAX_IP_LEN];
	struct broadcast_list *next;
} broadcast_head;

struct host_info local_host;

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

int msg_send(const char* ip_addr, int command, const char *message, const char* message_ex)
{
	struct net_address addr;
	char buff[MAX_UDPBUF];
	int buff_len = MAX_UDPBUF;

	strcpy_s(addr.ip, sizeof(addr.ip), ip_addr);
	addr.port = cfg_bind_port();

	if(!make_msg(command, message, message_ex, buff, &buff_len))
		return 0;

	return net_send_to(&addr, buff, buff_len);
}

int msg_recv(struct msg_packet *packet)
{
	struct net_address addr;
	char buff[MAX_UDPBUF];
	int buff_len;

	buff_len = net_recv_from(&addr, buff, MAX_UDPBUF);
	if(buff_len <= 0)
		return 0;

	buff[buff_len] = 0;
	packet->host.addr = addr;
	return parse_msg(buff, buff_len, packet);
}

int broadcast(int command, const char* msg)
{
	struct broadcast_list *broad_ptr = &broadcast_head;

	while(broad_ptr) {
		if(!msg_send(broad_ptr->ip, command, msg, ""))
			return 0;
		broad_ptr = broad_ptr->next;
	}

	return 1;
}

int process_msg(struct msg_packet *packet)
{
	int i;
	for (i=0; i<cmd_table_count; ++i)
	{
		if(cmd_table[i].cmd == (uint8)packet->command)
		{
			cmd_table[i].handle_func(packet);
			return 1;
		}
	}
	return 0;
}
