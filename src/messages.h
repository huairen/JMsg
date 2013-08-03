#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "user.h"

enum user_status {
    USER_STATUS_ONLINE,
    USER_STATUS_OFFLINE,
    USER_STATUS_LEAVE,
};

struct msg_packet {
	struct host_info host;
	int version;
	int id;
	int command;
	char msg[MAX_UDPBUF];
	char msg_ex[MAX_UDPBUF];
};

int msg_init();
int msg_recv(struct msg_packet *packet);

void notify_status(enum user_status);


int broadcast(int cmd, const char* msg);


#endif