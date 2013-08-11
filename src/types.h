#ifndef TYPES_H_
#define TYPES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <time.h>
#include <Windows.h>

#define strdup _strdup

#else
#include <unistd.h>
#include <netdb.h>

#define strcpy_s(dest,dest_len,src) strlcpy(dest,src,dest_len)
#define sprintf_s snprintf

#endif

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

#endif