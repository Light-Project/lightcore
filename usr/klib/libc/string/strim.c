#include <string.h>

char * strim(char * s)
{
	char * e;

	if(s)
	{
		while(isspace(*s))
			s++;
		if(*s == 0)
			return s;
		e = s + strlen(s) - 1;
		while((e > s) && isspace(*e))
			e--;
		*(e + 1) = 0;
	}
	return s;
}
