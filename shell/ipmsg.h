#ifndef IPMSG_H_
#define IPMSG_H_

#include "ipmsg_cmd.h"
#include "jmsg.h"

void ipmsg_init();
int process_msg(struct msg_packet *packet);
int make_msg(int command, const char *msg, const char *msg_ex, char *buff, int *buff_len);
int parse_msg(char *buff, int buff_len, struct msg_packet *packet);

void broadcast_status(int status);

int get_local_user_id();
const char* get_local_user_name();

#endif