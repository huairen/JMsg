#include "ipmsg.h"
#include "config.h"

extern uint32 hashlittle(const void *key, size_t length, uint32 initval);

int handle_user_entry(struct msg_packet *packet)
{
	uint32 user_id;
	struct user_info *user;

	user_id = hashlittle(packet->host_name, strlen(packet->host_name), 0);
	user = user_add(user_id);

	if(user) {
		strcpy_s(user->nick_name, sizeof(user->nick_name), packet->msg);
		strcpy_s(user->group_name, sizeof(user->group_name), packet->msg_ex);
	}

	msg_send(packet->addr.ip, IPMSG_ANSENTRY, cfg_nick_name(), cfg_group_name());

	return user_id;
}

int handle_user_exit( struct msg_packet *packet )
{
	uint32 user_id = hashlittle(packet->host_name, strlen(packet->host_name), 0);
	user_remove(user_id);
	return user_id;
}

int handle_answer_entry(struct msg_packet *packet)
{
	uint32 user_id;
	struct user_info *user;

	user_id = hashlittle(packet->host_name, strlen(packet->host_name), 0);
	user = user_add(user_id);

	if(user) {
		strcpy_s(user->nick_name, sizeof(user->nick_name), packet->msg);
		strcpy_s(user->group_name, sizeof(user->group_name), packet->msg_ex);
		return user_id;
	}

	return 0;
}

int handle_send_msg(struct msg_packet *packet)
{
	uint32 user_id;
	char buff[64];
	sprintf_s(buff, sizeof(buff), "%d", packet->time);

	if ((packet->command & IPMSG_SENDCHECKOPT) &&
		(packet->command & (IPMSG_BROADCASTOPT | IPMSG_AUTORETOPT)) == 0) {
			msg_send(packet->addr.ip, IPMSG_RECVMSG, buff, NULL);
	}

	user_id = hashlittle(packet->host_name, strlen(packet->host_name), 0);
	user_push_msg(user_id, user_id, packet->time, packet->msg);
	return user_id;
}

int handle_recv_msg( struct msg_packet *packet )
{
	return 0;
}