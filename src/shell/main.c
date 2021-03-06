#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <process.h>
#include "ipmsg.h"
#include "ipmsg_cmd.h"
#include "console.h"
#include "shell.h"

#define getch _getch

char buff[1024] = {0};

void recv_func(void* arg)
{
	uint32_t user_id;
	struct msg_packet msg;
	while(1)
	{
		msg_recv(&msg);
		user_id = process_msg(&msg);

		switch (GET_MODE(msg.command))
		{
		case IPMSG_SENDMSG:
			console_clear_line(-1);
 			shell_recv_msg(user_id);
			printf("%s", buff);
			break;

		case IPMSG_ANSENTRY:
		case IPMSG_BR_ENTRY:
			shell_user_entry(user_id);

		case IPMSG_BR_EXIT:
			console_clear_line(-1);
			shell_user_exit(user_id, msg.msg);
			printf("%s", buff);
			break;
		}
	}
}

int main(int argc, char* argv[])
{
	char c;
	int is_wchar = 0;

	ipmsg_init();
	shell_parse_cmd("list");

 	_beginthread(recv_func,0,0);

	while(1)
	{
		int buff_pos = 0;
		memset(buff, 0, sizeof(buff));

		fflush(stdout);
		while((c = getch()) != '\r')
		{
			if(!is_wchar) {
				if(c == 0 || c == -32) {
					getch();
					continue;
				}
			}

			switch(c)
			{
			case '\b':
				{
					if(buff_pos == 0)
						break;

					if(!isascii(buff[--buff_pos]))
						--buff_pos;

					if(buff_pos < 0)
						buff_pos = 0;

					buff[buff_pos] = 0;
					printf("\r%s  \r%s",buff,buff);
				}
				break;
			default:
				{
					buff[buff_pos++] = c;
					printf("%c",c);

					if(!isascii(c))
						is_wchar = !is_wchar;
				}
				break;
			}
		}

		if(buff[0])
		{
			console_clear_line(-1);

			if(buff[0] == '-')
			{
				if(_stricmp(buff+1, "exit") == 0)
				{
					broadcast_status(IPMSG_BR_EXIT);
					break;
				}

				shell_parse_cmd(buff+1);
				continue;
			}

			shell_self_say(buff);
		}
	}

	user_clear();
	return 0;
}