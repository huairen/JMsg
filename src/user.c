#include "user.h"
#include <stdio.h>

static struct user_list *user_head = NULL;

void user_add(struct user_info* user)
{
	struct user_list* new_node;

	if (user == NULL)
		return;

	new_node = (struct user_list*)malloc(sizeof(struct user_list));
	new_node->info = *user;
	new_node->next = user_head;
	user_head = new_node;
}

void user_list_dump()
{
	if (user_head != NULL)
	{
		struct user_list *user_ptr = user_head;

		while(user_ptr) {
			printf("1 %s\n",user_ptr->info.host.user_name);
			printf("2 %s\n",user_ptr->info.host.host_name);
			printf("3 %d\n",user_ptr->info.host.addr);
			printf("4 %d\n",user_ptr->info.host.port);

			printf("5 %s\n",user_ptr->info.nick_name);
			printf("6 %s\n",user_ptr->info.group_name);
			printf("7 %s\n",user_ptr->info.alter_name);
			printf("8 %d\n",user_ptr->info.status);
			printf("----------------------------------------\n");

			user_ptr = user_ptr->next;
		}
		printf("----------------------------------------\n");
	}
}

void user_remove( struct user_info* user )
{
	struct user_list *prev_ptr, *user_ptr = user_head;

	prev_ptr = NULL;
	while(user_ptr && user_ptr->info.host.addr != user->host.addr)
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

void user_ClearList()
{
	struct user_list *user_ptr;

	while (user_head)
	{
		user_ptr = user_head;
		user_head = user_head->next;
		free(user_ptr);
	}
}

struct user_info* user_find_by_addr( uint32 *addr )
{
	struct user_list *user_ptr = user_head;

	while (user_ptr)
	{
		if (user_ptr->info.host.addr == *addr)
		{
			return &user_ptr->info;
		}
		user_ptr = user_ptr->next;
	}
	printf("User not found in the user_list!!!\n");
	return NULL;
}

struct user_info* user_find_by_name( const char *user_name )
{
	struct user_list *user_ptr = user_head;

	while (user_ptr)
	{
		if (!strcmp(&user_ptr->info.host.user_name,user_name))
		{
			printf("find it!!!\n");
			return &user_ptr->info;
		}
		user_ptr = user_ptr->next;
	}
	printf("User not found in the user_list!!!\n");
	return NULL;
}