#include "miscfunc.h"

char *separate_token(char *buf, char separetor, char **handle)
{
	char *_handle;

	if (handle == NULL)
		handle = &_handle;

	if (buf)
		*handle = buf;

	if (*handle == NULL || **handle == 0)
		return	NULL;

	while (**handle == separetor)
		(*handle)++;
	buf = *handle;

	if (**handle == 0)
		return	NULL;

	while (**handle && **handle != separetor)
		(*handle)++;

	if (**handle == separetor)
		*(*handle)++ = 0;

	return	buf;
}

int local_to_unix(const char *src, char *dest, int dest_len)
{
	char *sv_dest = dest;
	char *max_dest = dest + dest_len - 1;

	while (*src && dest < max_dest) {
		if ((*dest = *src++) != '\r') dest++;
	}
	*dest = 0;

	return	(int)(dest - sv_dest);
}

int unix_to_local(const char *src, char *dest, int dest_len)
{
	char *sv_dest = dest;
	char *max_dest = dest + dest_len - 1;

	while (*src && dest < max_dest) {
		if ((*dest = *src++) == '\n' && dest + 1 < max_dest) {
			*dest++ = '\r';
			*dest++ = '\n';
		}
		else dest++;
	}
	*dest = 0;

	return (int)(dest - sv_dest);
}

static int _wtoa(const WCHAR *src, char *dst, int dst_len)
{
	int affect_len, len;

	if (dst_len == 1) {
		*dst = 0;
		return	0;
	}

	affect_len = dst_len ? dst_len - 1 : 0;
	len = WideCharToMultiByte(CP_ACP, 0, src, -1, dst, affect_len, 0, 0);

	if (len == 0 && dst && dst_len > 0) {
		if ((len = (int)strnlen(dst, affect_len)) == affect_len) {
			dst[len] = 0;
		}
	}

	return	len;
}

static int _u8tow(const char* src, WCHAR *dst, int dst_len)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, dst_len);

	if (len == 0 && dst && dst_len > 0) {
		if ((len = (int)wcsnlen(dst, dst_len)) == dst_len) {
			dst[--len] = 0;
		}
	}

	return	len;
}

char* w_to_a(const WCHAR* src)
{
	char *buf = NULL;
	int len;

	if ((len = _wtoa(src, NULL, 0)) > 0) {
		buf = (char*)malloc(len + 1);
		_wtoa(src, buf, len);
	}
	return	buf;
}

WCHAR* u8_to_w(const char* src)
{
	WCHAR *wbuf = NULL;
	int len;
	if ((len = _u8tow(src, NULL, 0)) > 0) {
		wbuf = (WCHAR*)malloc(sizeof(WCHAR) * (len + 1));
		_u8tow(src, wbuf, len);
	}
	return	wbuf;
}

char* u8_to_a(const char* src)
{
	char *buf = NULL;
	WCHAR *wsrc = u8_to_w(src);
	if (wsrc) {
		buf = w_to_a(wsrc);
	}
	free(wsrc);
	return	buf;
}