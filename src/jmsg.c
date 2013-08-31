#include "jmsg.hxx"
#include "network.hxx"
#include "config.hxx"

struct broadcast_list{
	char ip[MAX_IP_LEN];
	struct broadcast_list *next;
} broadcast_head;

static char local_user_name[MAX_NAME_LEN] = {0};
static char local_host_name[MAX_NAME_LEN] = {0};
static struct net_address local_addr;
static make_packet_handler make_packet_func = NULL;
static parse_packet_handler parse_packet_func = NULL;

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
	char host[MAX_NAME_LEN];
	struct hostent* hostinfo;

	if(!net_init())
		return 0;

	if(gethostname(host, sizeof(host)) == -1)
		host[0] = 0;
    
    //get hostname and username
#ifdef _WIN32
	{
        uint32_t name_len = sizeof(local_host_name);
        if(!GetComputerName(local_host_name, &name_len))
            return 0;

        name_len = sizeof(local_user_name);
        if(!GetUserName(local_user_name, &name_len))
            return 0;
    }
#else
	if(host[0] == 0)
		return 0;

	strcpy_s(local_host_name, sizeof(local_host_name), host);
    if(getlogin_r(local_user_name, sizeof(local_user_name)) != 0)
        return 0;
#endif

	hostinfo = gethostbyname(host);
    if(hostinfo) {
		int device_index = cfg_net_device();
		if(device_index >= hostinfo->h_length-1)
			device_index = 0;

		inet_ntop(hostinfo->h_addrtype, hostinfo->h_addr_list[0], local_addr.ip, sizeof(local_addr.ip));
	}
	
	if(net_udp_open(cfg_bind_port(), 0) != NET_ERR_SUCCESS)
		return 0;

	local_addr.port = cfg_bind_port();
	if(net_bind(net_udp_socket(), &local_addr))
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

	if(make_packet_func == NULL)
		return 0;

	strcpy_s(addr.ip, sizeof(addr.ip), ip_addr);
	addr.port = cfg_bind_port();

	if(!make_packet_func(command, message, message_ex, buff, &buff_len))
		return 0;

	return net_send_to(&addr, buff, buff_len);
}

int msg_recv(struct msg_packet *packet)
{
	char buff[MAX_UDPBUF];
	int buff_len;

	if(packet == NULL || parse_packet_func == NULL)
		return 0;

	buff_len = net_recv_from(&packet->addr, buff, MAX_UDPBUF);
	if(buff_len <= 0)
		return 0;

	buff[buff_len] = 0;
	return parse_packet_func(buff, buff_len, packet);
}

int broadcast(int command, const char* msg, const char* msg_ex)
{
	struct broadcast_list *broad_ptr = &broadcast_head;

	while(broad_ptr) {
		if(!msg_send(broad_ptr->ip, command, msg, ""))
			return 0;
		broad_ptr = broad_ptr->next;
	}

	return 1;
}

void msg_packet_handle( make_packet_handler make, parse_packet_handler parse )
{
	make_packet_func = make;
	parse_packet_func = parse;
}


const char* msg_local_user_name()
{
	return local_user_name;
}

const char* msg_local_host_name()
{
	return local_host_name;
}
