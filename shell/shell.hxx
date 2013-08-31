#ifndef SHELL_H_
#define SHELL_H_
#include "types.hxx"

int shell_parse_cmd(const char* cmd);
void shell_self_say(const char* text);
void shell_recv_msg(uint32_t user_id);
void shell_user_entry(uint32_t user_id);
void shell_user_exit(uint32_t user_id, const char* name);

#endif