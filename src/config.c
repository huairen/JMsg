#include "config.h"
#include "types.h"
#include "jmsg.h"
#include "user.h"

static int bind_port = 2425;
static int broadcast_mode = DIRECT_BROADCAST;
static int net_device_index = 0;
static char nick_name[MAX_NAME_LEN] = {0};
static char group_name[MAX_NAME_LEN] = {0};

static char* squish( char* src )
{
	char* cp = 0;

	if(src[0] == 0)
		return src;

	if(src == 0)
		return 0;

	for (cp = src + strlen(src) - 1; cp != src; --cp)
		if(!isspace(*(unsigned char*)cp))
			break;
	cp[1] = '\0';

	for (cp = src; isspace(*(unsigned char*)cp); ++cp)
		continue;

	return cp;
}

int cfg_load( const char* filename )
{
	char *line, *end, *name, *value;
	char buffer[4096];
	FILE *pf;

	if(fopen_s(&pf, filename, "rb"))
		return 0;
	
	while(fgets(buffer, sizeof(buffer), pf))
	{
		line = squish(buffer);
		if(line[0] == ';' || line[0] == '#' || line[0] == '\0')
			continue;

		end = strchr(line,'=');
		if(end == 0)
			break;

		*end++ = '\0';

		name = squish(line);
		value = squish(end);

		if(_stricmp(name, "nick_name") == 0)
			strcpy_s(nick_name, sizeof(nick_name), value);
		else if(_stricmp(name, "group_name") == 0)
			strcpy_s(group_name, sizeof(group_name), value);
		else if(_stricmp(name, "bind_port") == 0)
			bind_port = atoi(value);
		else if(_stricmp(name, "broadcast_mode") == 0)
			broadcast_mode = atoi(value);
	}

	fclose(pf);
	return 1;
}

int cfg_bind_port()
{
	return bind_port;
}

int cfg_broadcast_mode()
{
	return broadcast_mode;
}

int cfg_net_device()
{
	return net_device_index;
}

const char* cfg_nick_name()
{
	return nick_name;
}

const char* cfg_group_name()
{
	return group_name;
}

void cfg_set_broadcast_mode(int mode)
{
	broadcast_mode = mode;
}