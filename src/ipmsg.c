#include "types.h"
#include "ipmsg.h"
#include "messages.h"
#include "miscfunc.h"

extern struct host_info local_host;

void handle_user_entry(struct msg_packet *packet);
void handle_answer_entry(struct msg_packet *packet);

struct cmd_handle cmd_table[] = {
	{IPMSG_BR_ENTRY, &handle_user_entry},
	{IPMSG_ANSENTRY, &handle_answer_entry},
};

const int cmd_table_count = sizeof(cmd_table) / sizeof(struct cmd_handle);

static uint32 host_status()
{

}

//IPMSG的报文格式：版本号:包编号:发送者姓名:发送者主机名:命令字:附加信息
int make_msg(int command, const char *msg, const char *msg_ex, char *buff, int *buff_len)
{
	int packet_len;
	int packet_id;
	int max_len;
	int ex_len;
	int cmd = GET_MODE(command);
	int is_br_cmd = cmd == IPMSG_BR_ENTRY ||
					cmd == IPMSG_BR_EXIT ||
					cmd == IPMSG_BR_ABSENCE;

	if(buff_len == 0)
		return 0;

	max_len = *buff_len;
	packet_id = (int)time(NULL) + 1;
	
	packet_len = sprintf_s(buff, max_len, "%d:%u:%s:%s:%u:", IPMSG_VERSION, packet_id, local_host.user_name, local_host.host_name, command);

	ex_len = strlen(msg_ex);
	if(ex_len + packet_len + 2 >= max_len)
		ex_len = 0;
	max_len -= ex_len;

	if(msg) {
		packet_len += local_to_unix(msg, buff + packet_len, max_len - packet_len);
	}
	
	packet_len++;
	if(ex_len) {
		memcpy(buff + packet_len, msg_ex, ex_len);
		packet_len += ex_len;
	}

	*buff_len = packet_len;
	return 1;
}

int parse_msg(char *buff, int buff_len, struct msg_packet *packet)
{
	char *ex_str = NULL, *tok, *p;
	char *user_name, *host_name;
	int len, ex_len = 0;
	int is_utf8;

	len = (int)strlen(buff);

	if(buff_len > len + 1) {
		ex_str = buff + len + 1;
		ex_len = (int)strlen(ex_str);
	}

	if((tok = separate_token(buff, ':', &p)) == NULL)
		return 0;
	if((packet->version = atoi(tok)) != IPMSG_VERSION)
		return 0;

	if((tok = separate_token(NULL, ':', &p)) == NULL)
		return 0;
	packet->time = atoi(tok);

	if ((user_name = separate_token(NULL, ':', &p)) == NULL)
		return 0;

	if ((host_name = separate_token(NULL, ':', &p)) == NULL)
		return 0;

	if((tok = separate_token(NULL, ':', &p)) == NULL)
		return 0;

	packet->command = atoi(tok);
	packet->msg[0] = 0;
	packet->msg_ex[0] = 0;

	is_utf8 = (packet->command & IPMSG_UTF8OPT);

	strcpy_s(packet->host.user_name, sizeof(packet->host.user_name), user_name);
	strcpy_s(packet->host.host_name, sizeof(packet->host.host_name), host_name);

	if((tok = separate_token(NULL, 0, &p)))
	{
		char *tmp = NULL;
		if(is_utf8)
			tmp = u8_to_a(tok);
		unix_to_local(tmp ? tmp : tok, packet->msg, sizeof(packet->msg));
		free(tmp);
	}

	if(ex_str) {
		if(ex_str[0] != '\n') {
			char *tmp = NULL;
			if(is_utf8)
				tmp = u8_to_a(ex_str);
			strcpy_s(packet->msg_ex, sizeof(packet->msg_ex), tmp ? tmp : tok);
			free(tmp);
		}
	}

	return 1;
}


void handle_user_entry(struct msg_packet *packet)
{

}

void handle_answer_entry(struct msg_packet *packet)
{

}