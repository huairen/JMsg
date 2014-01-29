#ifndef MISCFUNC_H_
#define MISCFUNC_H_

#include "types.h"


#ifdef __cplusplus
extern "C" {
#endif

char *separate_token(char *buf, char separetor, char **handle);
int local_to_unix(const char *src, char *dest, int dest_len);
int unix_to_local(const char *src, char *dest, int dest_len);
char* w_to_a(const WCHAR* src);
WCHAR* u8_to_w(const char* src);
char* u8_to_a(const char* src);

#ifdef __cplusplus
};
#endif

#endif