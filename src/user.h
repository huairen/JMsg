#ifndef USER_H_
#define USER_H_

#include "types.h"
#include "network.h"

#define MAX_MSG_LEN 1024
#define MAX_NAME_LEN 80

enum user_status {
	USER_STATUS_ONLINE,
	USER_STATUS_OFFLINE,
	USER_STATUS_LEAVE,
};

struct user_message {
	uint32 id;
	uint32 time;
	char* text;
};

struct user_info {
	uint32 id;
	uint32 status;

	char nick_name[MAX_NAME_LEN];
	char group_name[MAX_NAME_LEN];
	char alter_name[MAX_NAME_LEN];

	struct net_address addr;
};

struct user_info* user_add(uint32 user_id);
struct user_info* user_find(uint32 user_id);
void user_remove(uint32 user_id);
void user_clear();
const char* user_show_name(uint32 user_id);

//history record
void user_push_msg(uint32 user_id, uint32 id, uint32 time, const char *text);
uint32 user_unrend_count(uint32 user_id);
struct user_message* user_unread_msg(uint32 user_id);
struct user_message* user_read_msg(uint32 user_id, int index);

#endif