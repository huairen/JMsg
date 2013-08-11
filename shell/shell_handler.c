#include "console.h"
#include "user.h"

// command handle
void handle_help(const char* arg)
{
}

void handle_refresh(const char* arg)
{
	memset(chat_user, 0, sizeof(chat_user));
	console_clear();
	user_clear();
	send_status(IPMSG_BR_ENTRY);
}

void handle_list(const char* arg)
{
	memset(chat_user, 0, sizeof(chat_user));
	console_clear();
	user_list_dump();
}

void handle_talk(const char* arg)
{
	int index;
	struct user_info *user;

	if(arg == NULL || arg[0] == 0)
		return;

	console_clear();

	index = atoi(arg);
	if(chat_user[0]) {
		if(index == 0)
			return;

		_talk_with_index(index);
		is_read_history = 1;
	} else {
		user = user_find_by_index(index-1);
		if(user != NULL && user != chat_user[0])
			_talk_with_user(user);
	}

	_show_chat_list();
	_show_chat_msg();
}