#include <stdio.h>

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
	int len = 0;

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