#ifndef JMSG_H_
#define JMSG_H_

#include "user.h"

#define MAX_UDPBUF 32768

typedef int (*make_packet_handler)(int,const char*,const char*,char *, int*);
typedef int (*parse_packet_handler)(char*, int, struct msg_packet*);

struct msg_packet {
	int version;
	int time;
	int command;
	char msg[MAX_UDPBUF];
	char msg_ex[MAX_UDPBUF];
	//host info
	char user_name[MAX_NAME_LEN];
	char host_name[MAX_NAME_LEN];
	struct net_address addr;
};

int msg_init();
int msg_send(const char* ip_addr, int command, const char *message, const char* message_ex);
int msg_recv(struct msg_packet *packet);

int broadcast(int command, const char* msg, const char* msg_ex);

void msg_packet_handle(make_packet_handler make, parse_packet_handler parse);
const char* msg_local_user_name();
const char* msg_local_host_name();

#endif