#include "config.h"
#include "types.h"
#include "messages.h"

static int bind_port = 2425;
static int broadcast_mode = DIRECT_BROADCAST;
static int net_device_index = 0;
static char nick_name[MAX_NAMELEN] = {0};
static char group_name[MAX_NAMELEN] = {0};

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
	return nick_name[0] ? nick_name : local_host.user_name;
}

const char* cfg_group_name()
{
	return group_name[0] ? group_name : NULL;
}

void cfg_set_broadcast_mode(int mode)
{
	broadcast_mode = mode;
}