#ifndef SHELL_H_
#define SHELL_H_
#include "types.h"

int shell_parse_cmd(const char* cmd);
void shell_self_say(const char* text);
void shell_recv_msg(uint32 user_id);
void shell_user_exit(uint32 user_id);

#endif