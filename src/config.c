#include "config.h"

static int bind_port = 2425;
static int broadcast_mode = LIMIT_BROADCAST;
static int net_device_index = 0;

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

void cfg_set_broadcast_mode(int mode)
{
	broadcast_mode = mode;
}