#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "user.h"

struct msg_packet {
	struct host_info host;
	int version;
	int time;
	int command;
	char msg[MAX_UDPBUF];
	char msg_ex[MAX_UDPBUF];
};

struct cmd_handle {
	uint8 cmd;
	void (*handle_func)(struct msg_packet*);
};

int msg_init();
int msg_send(const char* ip_addr, int command, const char *message, const char* message_ex);
int msg_recv(struct msg_packet *packet);
int broadcast(int command, const char* msg, const char* msg_ex);
int process_msg(struct msg_packet *packet);

void send_status(int command);

extern struct host_info local_host;

#endif