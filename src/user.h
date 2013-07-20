#ifndef USER_H_
#define USER_H_

#include "types.h"

struct user_info
{
	char name[NAME_LEN];
	char ip[32];
	uint32 status;
};

struct user_list
{
	struct user_list* next;
	struct user_info info;
};

void user_add();
void user_remove();
struct user_info* user_find();


#endif