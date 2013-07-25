#include <stdio.h>
#include <string.h>
#include "messages.h"

int main (int argc, char const *argv[])
{
	printf("Hello World\n");

    msg_init();
    notify_status(224395264);
    
	return 0;
}