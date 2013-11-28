#ifndef IPMSG_H_
#define IPMSG_H_

#include "ipmsg_cmd.hxx"
#include "jmsg.hxx"

#ifdef __cplusplus
extern "C" {
#endif

void ipmsg_init();
int process_msg(struct msg_packet *packet);
void broadcast_status(int status);

int get_local_user_id();
const char* get_nick_name();

#ifdef __cplusplus
};
#endif

#endif