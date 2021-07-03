#include <export.h>
#include <ctype.h>

int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}
EXPORT_SYMBOL(isalnum);

int isalpha(int c)
{
	return (((unsigned)c | 32) - 'a') < 26;
}
EXPORT_SYMBOL(isalpha);

int isascii(int c)
{
	return !(c & ~0x7f);
}
EXPORT_SYMBOL(isascii);

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}
EXPORT_SYMBOL(isblank);

int iscntrl(int c)
{
	return ((unsigned)c < 0x20) || (c == 0x7f);
}
EXPORT_SYMBOL(iscntrl);

int isdigit(int c)
{
	return ((unsigned)c - '0') < 10;
}
EXPORT_SYMBOL(isdigit);

int isgraph(int c)
{
	return ((unsigned)c - 0x21) < 0x5e;
}
EXPORT_SYMBOL(isgraph);

int islower(int c)
{
	return ((unsigned)c - 'a') < 26;
}
EXPORT_SYMBOL(islower);

int isprint(int c)
{
	return ((unsigned)c - 0x20) < 0x5f;
}
EXPORT_SYMBOL(isprint);

int ispunct(int c)
{
	return isgraph(c) && !isalnum(c);
}
EXPORT_SYMBOL(ispunct);

int isspace(int c)
{
	return (c == ' ') || ((unsigned)c - '\t' < 5);
}
EXPORT_SYMBOL(isspace);

int isupper(int c)
{
	return ((unsigned)c - 'A') < 26;
}
EXPORT_SYMBOL(isupper);

int isxdigit(int c)
{
	return isdigit(c) || (((unsigned)c | 32) - 'a' < 6);
}
EXPORT_SYMBOL(isxdigit);

int toascii(int c)
{
	return (c & 0x7f);
}
EXPORT_SYMBOL(toascii);

int tolower(int c)
{
	if(isupper(c))
		return c | 32;
	return c;
}
EXPORT_SYMBOL(tolower);

int toupper(int c)
{
	if(islower(c))
		return c & 0x5f;
	return c;
}
EXPORT_SYMBOL(toupper);
