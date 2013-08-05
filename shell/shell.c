#include <string.h>

struct shell_handle {
	char type[16];
	void (*handle_func)(const char*);
};

void handle_list(const char*);

static const struct shell_handle table[] = {
	{"list", &handle_list},
};

static const int table_count = sizeof(table) / sizeof(struct shell_handle);

int shell_parse_cmd(const char* cmd)
{
	int i;

	for (i=0; i<table_count; ++i)
	{
		if(strcmp(table[i].type, cmd) == 0)
		{
			table[i].handle_func(cmd);
			return 1;
		}
	}
	return 0;
}

void handle_list(const char* arg)
{
}