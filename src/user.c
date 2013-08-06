#include "user.h"

static struct user_list *user_head = NULL;

void user_add(struct user_info* user)
{
	struct user_list* new_node;
	struct user_info* old_user;

	if (user == NULL)
		return;

	old_user = user_find_by_host(&user->host);
	if(old_user) {
		*old_user = *user;
		return;
	}

	new_node = (struct user_list*)malloc(sizeof(struct user_list));
	new_node->info = *user;
	new_node->next = user_head;
	user_head = new_node;
}

void user_remove( struct user_info* user )
{
	struct user_list *prev_ptr, *user_ptr = user_head;

	prev_ptr = NULL;
	while(user_ptr && !user_is_same(&user_ptr->info.host, &user->host))
	{
		prev_ptr = user_ptr;
		user_ptr = user_ptr->next;
	}

	if(prev_ptr != NULL)
		prev_ptr->next = user_ptr->next;
	else
		user_head = user_ptr->next;

	free(user_ptr);
}

void user_clear()
{
	struct user_list *user_ptr;
	while (user_head)
	{
		user_ptr = user_head;
		user_head = user_head->next;
		free(user_ptr);
	}
}

struct user_info* user_find_by_host( struct host_info *host )
{
	struct user_list *user_ptr = user_head;

	while (user_ptr) {
		if (user_is_same(host, &(user_ptr->info.host)))
			return &user_ptr->info;
		user_ptr = user_ptr->next;
	}
	return NULL;
}

struct user_info* user_find_by_index(int index)
{
	int i;
	struct user_list *user_ptr = user_head;

	for(i=0; i<index && user_ptr; user_ptr = user_ptr->next, ++i);

	return &user_ptr->info;
}

const char* user_show_name( struct user_info *user )
{
	if(user == NULL)
		return NULL;

	if(user->alter_name[0])
		return user->alter_name;

	if(user->nick_name[0])
		return user->nick_name;

	return user->host.host_name;
}

int user_is_same( struct host_info *host1, struct host_info *host2 )
{
	if (_stricmp(host1->host_name, host2->host_name))
		return	0;

	return	_stricmp(host1->user_name, host2->user_name) ? 0 : 1;
}


void user_list_dump()
{
	int i = 1;
	struct user_list *user_ptr = user_head;
	const char *name;

	while(user_ptr) {
		name = user_ptr->info.nick_name;
		printf("%2d. %-20s %-20s %s\n", i++,
			*name ? name : user_ptr->info.host.user_name,
			user_ptr->info.host.host_name,
			user_ptr->info.host.addr.ip);
		user_ptr = user_ptr->next;
	}
}