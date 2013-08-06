#include <string.h>
#include "user.h"
#include "console.h"
#include "messages.h"
#include "ipmsg.h"

struct shell_handle {
	char type[16];
	int type_len;
	void (*handle_func)(const char*);
};

void handle_help(const char*);
void handle_refresh(const char*);
void handle_list(const char*);
void handle_talk(const char*);

static const struct shell_handle table[] = {
	{"help", 4, &handle_help},
	{"refresh", 7, &handle_refresh},
	{"list", 4, &handle_list},
	{"talk", 4, &handle_talk},
};

static const int table_count = sizeof(table) / sizeof(struct shell_handle);

struct user_info talk_user;

int shell_parse_cmd(const char* cmd)
{
	int i;

	for (i=0; i<table_count; ++i)
	{
		if(strncmp(table[i].type, cmd, table[i].type_len) == 0)
		{
			table[i].handle_func(cmd + table[i].type_len + 1);
			return 1;
		}
	}
	return 0;
}

void handle_help(const char* arg)
{

}

void handle_refresh(const char* arg)
{
	memset(&talk_user, 0, sizeof(talk_user));
	console_clear();
	user_clear();
	send_status(IPMSG_BR_ENTRY);
}

void handle_list(const char* arg)
{
	memset(&talk_user, 0, sizeof(talk_user));
	console_clear();
	user_list_dump();
}

void handle_talk(const char* arg)
{
	int index = atoi(arg);
	struct user_info *user = user_find_by_index(index-1);

	if(user != NULL)
	{
		talk_user = *user;
		console_clear();
		printf("# talk with %s\n", user->nick_name[0] ? user->nick_name : user->host.user_name);
	}
}