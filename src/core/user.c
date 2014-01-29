#include "user.h"
#include "config.h"

struct user_message_list {
	struct user_message_list *next;
	struct user_message data;
};

struct user_list {
	struct user_info info;
	uint32_t last_read_time;
	struct user_message_list *messages;
	struct user_list* next;
};

static struct user_list *user_head = NULL;

static void user_message_free(struct user_list *user)
{
	struct user_message_list *next;

	while(user->messages) {
		next = user->messages->next;
		free(user->messages->data.text);
		free(user->messages);
		user->messages = next;
	}
}

static void user_list_free(struct user_list *user)
{
	user_message_free(user);
	free(user);
}

static struct user_list* user_list_find(uint32_t id)
{
	struct user_list *user_ptr = user_head;
	while (user_ptr) {
		if (user_ptr->info.id == id)
			return user_ptr;
		user_ptr = user_ptr->next;
	}
	return NULL;
}

struct user_info* user_add(uint32_t user_id)
{
	struct user_list* new_user;

	new_user = user_list_find(user_id);
	if(new_user == NULL) {
		new_user = (struct user_list*)malloc(sizeof(struct user_list));
		memset(new_user, 0, sizeof(struct user_list));

		new_user->info.id = user_id;
		new_user->next = user_head;
		user_head = new_user;
	}

	return &new_user->info;
}

struct user_info* user_find( uint32_t user_id )
{
	struct user_list* find_user = user_list_find(user_id);
	if(find_user == NULL)
		return NULL;
	return &find_user->info;
}

struct user_info* user_find_by_index( uint32_t index )
{
	struct user_list *user_ptr = user_head;
	while (user_ptr) {
		if (index-- == 0)
			return &user_ptr->info;
		user_ptr = user_ptr->next;
	}
	return NULL;
}

struct user_info* user_find_first()
{
	return &user_head->info;
}

struct user_info* user_find_next( struct user_info* user )
{
	struct user_list *user_node = (struct user_list*)user;
	user_node = user_node->next;
	return &user_node->info;
}

void user_remove(uint32_t user_id)
{
	struct user_list *prev_ptr, *user_ptr = user_head;

	prev_ptr = NULL;
	while(user_ptr && (user_ptr->info.id != user_id))
	{
		prev_ptr = user_ptr;
		user_ptr = user_ptr->next;
	}

	if(user_ptr == NULL)
		return;

	if(prev_ptr != NULL)
		prev_ptr->next = user_ptr->next;
	else
		user_head = user_ptr->next;

	user_list_free(user_ptr);
}

void user_clear()
{
	struct user_list *user_ptr;
	while (user_head)
	{
		user_ptr = user_head;
		user_head = user_head->next;
		user_list_free(user_ptr);
	}
}

const char* user_show_name( uint32_t user_id )
{
	struct user_list* find_user = user_list_find(user_id);
	if(find_user == NULL)
		return NULL;
	
	if(find_user->info.alter_name[0])
		return find_user->info.alter_name;

	if(find_user->info.nick_name[0])
		return find_user->info.nick_name;

	return NULL;
}

//user message
void user_push_msg( uint32_t user_id, uint32_t id, uint32_t time, const char *text )
{
	struct user_message_list *msg;
	struct user_list* find_user;

	find_user = user_list_find(user_id);
	if(find_user == NULL)
		return;
	
	msg = (struct user_message_list*)malloc(sizeof(struct user_message_list));
	if(msg == NULL)
		return;

	msg->data.id = id;
	msg->data.time = time;
	msg->data.text = strdup(text);
	msg->next = find_user->messages;
	find_user->messages = msg;
}

uint32_t user_unrend_count( uint32_t user_id )
{
	struct user_list *find_user;
	struct user_message_list *msg_ptr;
	int count = 0;

	find_user = user_list_find(user_id);
	if(find_user == NULL)
		return 0;

	msg_ptr = find_user->messages;
	while(msg_ptr) {
		if(msg_ptr->data.time <= find_user->last_read_time)
			break;

		++count;
		msg_ptr = msg_ptr->next;
	}

	return count;
}

struct user_message* user_unread_msg( uint32_t user_id )
{
	struct user_list *find_user;
	struct user_message_list *msg_ptr, *prev_msg = NULL;

	find_user = user_list_find(user_id);
	if(find_user == NULL)
		return 0;

	msg_ptr = find_user->messages;
	while(msg_ptr) {
		if(msg_ptr->data.time <= find_user->last_read_time)
			break;

		prev_msg = msg_ptr;
		msg_ptr = msg_ptr->next;
	}
	
	if(prev_msg == NULL)
		return NULL;

	find_user->last_read_time = prev_msg->data.time;
	return &prev_msg->data;
}

struct user_message* user_read_msg( uint32_t user_id, int index )
{
	struct user_list *find_user;
	struct user_message_list *msg_ptr;
	int count = 0;

	find_user = user_list_find(user_id);
	if(find_user == NULL)
		return 0;

	msg_ptr = find_user->messages;
	while(msg_ptr) {
		if(msg_ptr->data.time <= find_user->last_read_time) {
			if(index-- <= 0)
				return &msg_ptr->data;
		}

		msg_ptr = msg_ptr->next;
	}

	return NULL;
}