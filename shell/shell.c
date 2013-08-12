#include "shell.h"
#include <string.h>
#include "user.h"
#include "console.h"
#include "ipmsg.h"
#include "config.h"
#include "jmsg.h"

#define MAX_CHAT_NUM 10

enum shell_status{
	SHELL_STATUS_NULL,
	SHELL_STATUS_LIST,
	SHELL_STATUS_TALK,
};

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

static uint32_t chat_user[MAX_CHAT_NUM] = {0};
static int is_read_history = 0;
static enum shell_status curr_status = SHELL_STATUS_NULL;

static void _show_chat_list()
{
	int i;
	int line = console_current_line();
	if(line == 0)
		++line;

	if(curr_status != SHELL_STATUS_TALK || chat_user[0] == 0)
		return;

	console_clear_line(0);
	printf("# talk with %s  ", user_show_name(chat_user[0]));

	for (i = 1; i<MAX_CHAT_NUM; ++i) {
		if(chat_user[i] != 0) {
			int cnt = user_unrend_count(chat_user[i]);
			if(cnt)
				printf("%d.%s(%d)", i, user_show_name(chat_user[i]), cnt);
			else
				printf("%d.%s", i, user_show_name(chat_user[i]));
		}
	}

	console_goto_xy(0,line);
}

static void _show_chat_msg()
{
	int i;
	struct user_message* his_ptr;

	if(chat_user[0] == 0 || curr_status != SHELL_STATUS_TALK)
		return;

	if(is_read_history) {
		for (i = 2; i >= 0; --i) {
			his_ptr = user_read_msg(chat_user[0], i);
			if(his_ptr)
				printf("%s: %s\n", user_show_name(his_ptr->id), his_ptr->text);
		}
	}

	his_ptr = user_unread_msg(chat_user[0]);
	while(his_ptr) {
		printf("%s: %s\n", user_show_name(his_ptr->id), his_ptr->text);
		his_ptr = user_unread_msg(chat_user[0]);
	}
}

static void _push_user(uint32_t user_id)
{
	int i;
	for (i = 0; i < MAX_CHAT_NUM; ++i) {
		if(chat_user[i]) {
			if(chat_user[i] == user_id)
				break;
		} else {
			chat_user[i] = user_id;
			break;
		}	
	}

	_show_chat_list();
}

static void _talk_with_user(uint32_t user_id)
{
	int i;
	for (i = MAX_CHAT_NUM - 1; i > 0; --i)
		chat_user[i] = chat_user[i-1];

	chat_user[0] = user_id;
}

static int _talk_with_index(int index)
{
	uint32_t user_id;
	
	if(index < 0 || index >= MAX_CHAT_NUM)
		return 0;

	user_id = chat_user[index];
	if(user_id == 0)
		return 0;

	for (; index > 0; --index)
		chat_user[index] = chat_user[index-1];

	chat_user[0] = user_id;
	return 1;
}


//----------------------------------------------------------------
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

void shell_self_say( const char* text )
{
	struct user_info *user;

	if(chat_user[0] == 0)
		return;

	user = user_find(chat_user[0]);
	if(user == NULL)
		return;
	
	msg_send(user->addr.ip,IPMSG_SENDMSG,text,NULL);
	user_push_msg(chat_user[0], get_local_user_id(), (int)time(NULL), text);

	console_set_text_color(FOREGROUND_RED);
	printf("%s: %s\n", get_nick_name(), text);
	console_set_text_color(0xffff);
}

void shell_recv_msg(uint32_t user_id)
{
	if(user_id != chat_user[0])
		_push_user(user_id);

	if(curr_status == SHELL_STATUS_TALK)
		_show_chat_msg();
	else
		printf("Tip: new message! \"-talk\" recv.\n");
}

void shell_user_exit(uint32_t user_id)
{
	int i;
	for (i = 0; i < MAX_CHAT_NUM; ++i) {
		if(chat_user[i] == 0)
			break;
		if(user_id == chat_user[i])
			break;
	}

	for (; i < MAX_CHAT_NUM - 1 ; ++i) {
		chat_user[i] = chat_user[i + 1];
	}

	_show_chat_list();
}


// command handle
void handle_help(const char* arg)
{
}

void handle_refresh(const char* arg)
{
	memset(chat_user, 0, sizeof(chat_user));
	console_clear();
	user_clear();
	broadcast_status(IPMSG_BR_ENTRY);
}

void handle_list(const char* arg)
{
	int i = 0;
	struct user_info *user;

	memset(chat_user, 0, sizeof(chat_user));
	console_clear();
	curr_status = SHELL_STATUS_LIST;

	printf("±àºÅ.%-18s %-10s %-16s\n", "Ãû×Ö", "×é", "IP");
	printf("---------------------------------------------------\n");
	
	user = user_find_first();
	while(user) {
		printf("%4d.%-18s %-10s %-16s\n", i, user_show_name(user->id), user->group_name, user->addr.ip);
		user = user_find_next(user);
		i++;
	}
}

void handle_talk(const char* arg)
{
	int index;

	if(arg != NULL && arg[0] != 0)	
	{
		index = atoi(arg);

		if(curr_status == SHELL_STATUS_LIST) {
			struct user_info *user = user_find_by_index(index);
			if(user == NULL || user->id == chat_user[0])
				return;

			_talk_with_user(user->id);
		} else {
			if(index == 0 || !_talk_with_index(index))
				return;

			is_read_history = 1;
		}
	}
	
	curr_status = SHELL_STATUS_TALK;
	console_clear();
	_show_chat_list();
	_show_chat_msg();
}