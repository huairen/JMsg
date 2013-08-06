#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <process.h>
#include "messages.h"
#include "ipmsg.h"
#include "user.h"
#include "config.h"
#include "console.h"

extern int shell_parse_cmd(const char* cmd);
extern struct user_info talk_user;

char buff[1024] = {0};
int last_packet_no = 0;

void recv_func(void* arg)
{
	struct msg_packet msg;
	while(1)
	{
		msg_recv(&msg);
		process_msg(&msg);

		switch (GET_MODE(msg.command))
		{
		case IPMSG_SENDMSG:
			if(last_packet_no != msg.time)
			{
				last_packet_no = msg.time;

				console_clear_line(-1);
				if(strcmp(talk_user.host.host_name, msg.host.host_name) == 0)
					printf("%s: %s\n", user_show_name(&talk_user), msg.msg);
				printf("%s",buff);
			}
			break;
		}
	}
}

int main(int argc, char* argv[])
{
	char c;
	char wchar[3] = {0};
	int is_ch = 0;

	msg_init();
	send_status(IPMSG_BR_ENTRY);

 	_beginthread(recv_func,0,0);

	while(1)
	{
		int buff_pos = 0;
		memset(buff, 0, sizeof(buff));

		fflush(stdout);
		while((c = getch()) != '\r')
		{
			switch(c)
			{
			case 0xE0:
				getch();
				break;
			case '\b':
				{
					if(buff_pos > 1 && buff[buff_pos - 1] < 0)
						--buff_pos;

					if(buff_pos > 0)
					{
						buff[--buff_pos] = 0;
						printf("\r%s  \r%s",buff,buff);
					}
				}
				break;
			default:
				{
					if(is_ch)
					{
						is_ch = 0;
						wchar[1] = c;
						printf("%s",wchar);
					}
					else if(c < 0)
					{
						is_ch = 1;
						wchar[0] = c;
					}
					else
					{
						printf("%c",c);
					}

					buff[buff_pos++] = c;
				}
				break;
			}
		}

		console_clear_line(-1);

		if(buff[0] == '-' && shell_parse_cmd(buff+1))
			continue;

		if(talk_user.host.addr.ip[0] != 0)
		{
			msg_send(talk_user.host.addr.ip,IPMSG_SENDMSG,buff,NULL);
			console_set_text_color(FOREGROUND_RED);
			printf("%s: %s\n", cfg_nick_name(), buff);
			console_set_text_color(0xffff);
		}
	}

	return 0;
}