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
	uint32_t id;
	uint32_t time;
	char* text;
};

struct user_info {
	uint32_t id;
	uint32_t status;

	char nick_name[MAX_NAME_LEN];
	char group_name[MAX_NAME_LEN];
	char alter_name[MAX_NAME_LEN];

	struct net_address addr;
};


#ifdef __cplusplus
extern "C" {
#endif

struct user_info* user_add(uint32_t user_id);
struct user_info* user_find(uint32_t user_id);
struct user_info* user_find_by_index(uint32_t index);
struct user_info* user_find_first();
struct user_info* user_find_next(struct user_info* user);

void user_remove(uint32_t user_id);
void user_clear();

const char* user_show_name(uint32_t user_id);

//history record
void user_push_msg(uint32_t user_id, uint32_t id, uint32_t time, const char *text);
uint32_t user_unrend_count(uint32_t user_id);
struct user_message* user_unread_msg(uint32_t user_id);
struct user_message* user_read_msg(uint32_t user_id, int index);


#ifdef __cplusplus
};
#endif

#endif