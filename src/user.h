#ifndef USER_H_
#define USER_H_

#include "types.h"
#include "network.h"

struct host_info {
    char user_name[MAX_NAMELEN];
    char host_name[MAX_NAMELEN];
    struct net_address addr;
};

struct user_info {
    struct host_info host;
	char nick_name[MAX_NAMELEN];
    char group_name[MAX_NAMELEN];
    char alter_name[MAX_NAMELEN];
	uint32 status;
};

struct user_list {
	struct user_list* next;
	struct user_info info;
};

void user_add(struct user_info* user);
void user_remove(struct user_info* user);
void user_clear();
struct user_info* user_find_by_host(struct host_info *host);
struct user_info* user_find_by_index(int index);

const char* user_show_name(struct user_info *user);
int user_is_same(struct host_info *user1, struct host_info *user2);

void user_list_dump();


#endif