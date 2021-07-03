#include <string.h>

void *memset(void * s, int c, size_t n)
{
	char * xs = s;

	while (n--)
		*xs++ = c;

	return s;
}
