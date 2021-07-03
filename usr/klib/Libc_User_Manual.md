# Libc User manual

## assert

### assert

## ctype

### isalnum

This function checks whether the passed character is alphanumeric.

### isalpha

This function checks whether the passed character is alphabetic.

### isascii

### isblank

### iscntrl

This function checks whether the passed character is control character.

### isdigit

This function checks whether the passed character is  decimal digit.

### isgraph

This function checks whether the passed character has graphical representation using locale.

### islower

This function checks whether the passed character is  lowercase letter.

### isprint

This function checks whether the passed character is printable.

### ispunct

This function checks whether the passed character is a punctuation character.

### isspace

This function checks whether the passed character is white-space.

### isupper

This function checks whether the passed character is an uppercase letter.

### isxdigit

This function checks whether the passed character is a hexadecimal digit.

### toascii

### tolower

### toupper

## math

### acos

### acosf

### acosh

### acoshf

### asin

### asinf

### asinh

### asinhf

### atan2

### atan2f

### atan

### atanf

### atanh

### atanhf

### cbrt

### cbrtf

### ceil

### ceilf

### cos

### cosf

### cosh

### coshf

### exp2

### exp2f

### exp

### expf

### expm1

### expm1f

### fabs

### fabsf

### fdim

### fdimf

### floor

### floorf

### fmod

### fmodf

### frexp

### frexpf

### hypot

### hypotf

### ldexp

### ldexpf

### log10

### log10f

### log1p

### log1pf

### log2

### log2f

### log

### logf

### modf

### modff

### pow

### powf

### rint

### rintf

### round

### roundf

### scalbln

### scalblnf

### scalbn

### scalbnf

### sin

### sinf

### sinh

### sinhf

### sqrt

### sqrtf

### tan

### tanf

### tanh

### tanhf

### trunc

### truncf

## stdio

### asprintf

### clearerr

### fclose

### feof

### ferror

### fflush

### fgetc

### fgetpos

### fgets

### fopen

### fprintf

### fputc

### fputs

### fread

### freopen

### fscanf

### fseek

### fsetpos

### ftell

### fwrite

### getc

### getchar

### printf

### putc

### putchar

### remove

### rename

### rewind

### scanf

### setbuf

### setvbuf

### snprintf

### sprintf

### sscanf

### system

### tmpfile

### tmpnam

### ungetc

### vasprintf

### vsnprintf

### vsscanf

## stdlib

### abs

Returns the absolute value of x.

```
int abs(int n)
{
	return ((n < 0) ? -n : n);
}
```



### atof

Converts the string pointed to, by the argument *str* to a floating-point number (type double).

```
double atof(const char * nptr)
{
	return (double)strtod(nptr, 0);
}
```



### atoi

Converts the string pointed to, by the argument *str* to an integer (type int).

```
int atoi(const char * nptr)
{
	return (int)strtol(nptr, NULL, 10);
}
```



### atol

Converts the string pointed to, by the argument *str* to a long integer (type long int).

```
long atol(const char * nptr)
{
	return (long)strtol(nptr, NULL, 10);
}
```



### atoll

```
long long atoll(const char * nptr)
{
	return (long long)strtoll(nptr, NULL, 10);
}
```



### bsearch

```
void * bsearch(const void * key, const void * base, size_t nmemb, size_t size,
		int (*compar)(const void *, const void *))
{
	size_t l, u, idx;
	const void *p;
	int comparison;

	l = 0;
	u = nmemb;
	while (l < u)
	{
		idx = (l + u) / 2;
		p = (void *) (((const char *) base) + (idx * size));
		comparison = (*compar)(key, p);
		if (comparison < 0)
			u = idx;
		else if (comparison > 0)
			l = idx + 1;
		else
			return (void *) p;
	}

	return NULL;
}
```



### div

Divides numer (numerator) by denom (denominator).

```
div_t div(int num, int den)
{
	return (div_t){ num / den, num % den };
}
```



### labs

Returns the absolute value of x.

```
long labs(long n)
{
	return ((n < 0L) ? -n : n);
}
```



### ldiv

Divides numer (numerator) by denom (denominator).

```
ldiv_t ldiv(long num, long den)
{
	return (ldiv_t){ num / den, num % den };
}
```



### llabs

```
long long llabs(long long n)
{
	return ((n < 0LL) ? -n : n);
}
```



### lldiv

```
lldiv_t lldiv(long long num, long long den)
{
	return (lldiv_t){ num / den, num % den };
}
```



### qsort

Sorts an array.

```
#define swapcode(TYPE, parmi, parmj, n) {	\
	long i = (n) / sizeof (TYPE);			\
	TYPE *pi = (TYPE *) (parmi);			\
	TYPE *pj = (TYPE *) (parmj);			\
	do {									\
		TYPE	t = *pi;					\
		*pi++ = *pj;						\
		*pj++ = t;							\
        } while (--i > 0);					\
}

#define swap(a, b)							\
	if (swaptype == 0) {					\
		long t = *(long *)(a);				\
		*(long *)(a) = *(long *)(b);		\
		*(long *)(b) = t;					\
	} else									\
		swapfunc(a, b, es, swaptype)

#define vecswap(a, b, n)					\
	if ((n) > 0)							\
		swapfunc(a, b, n, swaptype)

#define SWAPINIT(a, es)						\
	swaptype = ((char *)a - (char *)0) % sizeof(long) || es % sizeof(long) ? 2 : es == sizeof(long)? 0 : 1;

static __inline char * med3(char *a, char *b, char *c, int (*cmp)(const void *, const void *))
{
	return cmp(a, b) < 0 ?
			(cmp(b, c) < 0 ? b : (cmp(a, c) < 0 ? c : a)) :
			(cmp(b, c) > 0 ? b : (cmp(a, c) < 0 ? a : c));
}

static __inline void swapfunc(char *a, char *b, int n, int swaptype)
{
	if (swaptype <= 1)
		swapcode(long, a, b, n)
	else
		swapcode(char, a, b, n)
}

void qsort(void * aa, size_t n, size_t es, int (*cmp)(const void *, const void *))
{
	char * pa, * pb, * pc, * pd, * pl, * pm, * pn;
	int d, r, swaptype, swap_cnt;
	char * a = aa;

loop:
	SWAPINIT(a, es);
	swap_cnt = 0;
	if(n < 7)
	{
		for(pm = (char *)a + es; pm < (char *)a + n * es; pm += es)
			for(pl = pm; pl > (char *)a && cmp(pl - es, pl) > 0; pl -= es)
				swap(pl, pl - es);
		return;
	}
	pm = (char *)a + (n / 2) * es;
	if(n > 7)
	{
		pl = (char *)a;
		pn = (char *)a + (n - 1) * es;
		if(n > 40)
		{
			d = (n / 8) * es;
			pl = med3(pl, pl + d, pl + 2 * d, cmp);
			pm = med3(pm - d, pm, pm + d, cmp);
			pn = med3(pn - 2 * d, pn - d, pn, cmp);
		}
		pm = med3(pl, pm, pn, cmp);
	}
	swap(a, pm);
	pa = pb = (char *)a + es;

	pc = pd = (char *)a + (n - 1) * es;
	for(;;)
	{
		while(pb <= pc && (r = cmp(pb, a)) <= 0)
		{
			if(r == 0)
			{
				swap_cnt = 1;
				swap(pa, pb);
				pa += es;
			}
			pb += es;
		}
		while(pb <= pc && (r = cmp(pc, a)) >= 0)
		{
			if(r == 0)
			{
				swap_cnt = 1;
				swap(pc, pd);
				pd -= es;
			}
			pc -= es;
		}
		if(pb > pc)
			break;
		swap(pb, pc);
		swap_cnt = 1;
		pb += es;
		pc -= es;
	}
	if(swap_cnt == 0)
	{
		for(pm = (char *)a + es; pm < (char *)a + n * es; pm += es)
			for(pl = pm; pl > (char *)a && cmp(pl - es, pl) > 0; pl -= es)
				swap(pl, pl - es);
		return;
	}

	pn = (char *)a + n * es;
	r = min(pa - (char * )a, pb - pa);
	vecswap(a, pb - r, r);
	r = min(pd - pc, pn - pd - (int )es);
	vecswap(pb, pn - r, r);
	if((r = pb - pa) > (int)es)
		qsort(a, r / es, es, cmp);
	if((r = pd - pc) > (int)es)
	{
		a = pn - r;
		n = r / es;
		goto loop;
	}
}
```



### rand

Returns a pseudo-random number in the range of 0 to *RAND_MAX*.



### strntoimax



### strntoumax



### strtod

Converts the string pointed to, by the argument *str* to a floating-point number (type double).

### strtoimax

### strtol

Converts the string pointed to, by the argument *str* to a long integer (type long int).

### strtoll

### strtoul

Converts the string pointed to, by the argument *str* to an unsigned long integer (type unsigned long int).



### strtoull



### strtoumax



## string

### memchr

Searches for the first occurrence of the character c (an unsigned char)  in the first n bytes of the string pointed to, by the argument *str*.



### memcmp

Compares the first n bytes of *str1* and *str2*.

```
static int memcmp(const void * s1, const void * s2, size_t n)
{
	const unsigned char *su1, *su2;
	int res = 0;

	for (su1 = s1, su2 = s2; 0 < n; ++su1, ++su2, n--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}
```



### memcpy

Copies n characters from src to *dest*.

```
static void * memcpy(void * dest, const void * src, size_t len)
{
	char * tmp = dest;
	const char * s = src;

	while (len--)
		*tmp++ = *s++;
	return dest;
}
```



### memmove

Another function to copy n characters from *str2* to *str1*.

```
static void * memmove(void * dest, const void * src, size_t n)
{
	char * tmp;
	const char * s;

	if (dest <= src)
	{
		tmp = dest;
		s = src;
		while (n--)
			*tmp++ = *s++;
	}
	else
	{
		tmp = dest;
		tmp += n;
		s = src;
		s += n;
		while (n--)
			*--tmp = *--s;
	}
	return dest;
}
```



### memscan

```
void * memscan(void * addr, int c, size_t size)
{
	unsigned char * p = addr;

	while (size)
	{
		if (*p == c)
			return (void *)p;
		p++;
		size--;
	}
  	return (void *)p;
}
```



### memset

Copies the character c (an unsigned char) to the first n characters of the string pointed to, by the argument *str*.

```
static void * memset(void * s, int c, size_t n)
{
	char * xs = s;

	while (n--)
		*xs++ = c;

	return s;
}
```



### strcasecmp

```
int strcasecmp(const char * s1, const char * s2)
{
	int c1, c2;

	do {
		c1 = tolower(*s1++);
		c2 = tolower(*s2++);
	} while (c1 == c2 && c1 != 0);

	return c1 - c2;
}
```



### strcat

Appends the string pointed to, by *src* to the end of the string pointed to by *dest*.

```
char * strcat(char * dest, const char * src)
{
	char * tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0');

	return tmp;
}
```



### strchr

Searches for the first occurrence of the character c (an unsigned char) in the string pointed to, by the argument *str*.

```
char * strchr(const char * s, int c)
{
	for (; *s != (char)c; ++s)
		if (*s == '\0')
			return NULL;
	return (char *)s;
}
```



### strcmp

Compares the string pointed to, by *str1* to the string pointed to by *str2*.

```
static int strcmp(const char * s1, const char * s2)
{
	int res;

	while (1)
	{
		if ((res = *s1 - *s2++) != 0 || !*s1++)
			break;
	}
	return res;
}
```



### strcoll

Compares string *str1* to *str2*. The result is dependent on the LC_COLLATE setting of the location.

```
int strcoll(const char * s1, const char * s2)
{
	return strcmp(s1, s2);
}
```



### strcpy

```
char * strcpy(char * dest, const char * src)
{
	char * tmp = dest;

	while ((*dest++ = *src++) != '\0');
	return tmp;
}
```



### strcspn

Calculates the length of the initial segment of str1 which consists entirely of characters not in str2.

```
size_t strcspn(const char * s, const char * reject)
{
	const char * p;
	const char * r;
	size_t count = 0;

	for (p = s; *p != '\0'; ++p)
	{
		for (r = reject; *r != '\0'; ++r)
		{
			if (*p == *r)
				return count;
		}
		++count;
	}
	return count;
}
```



### strdup

```
char * strdup(const char * s)
{
	char * p;

	if(!s)
		return NULL;

	p = malloc(strlen(s) + 1);
	if(p)
		return(strcpy(p, s));

	return NULL;
}
```



### strim

```
char * strim(char * s)
{
	char * e;

	if(s
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
```



### strlcat

### strlcpy

```
size_t strlcpy(char * dest, const char * src, size_t n)
{
	size_t len;
	size_t ret = strlen(src);

	if (n)
	{
		len = (ret >= n) ? n - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}
```

### strlen

### strncasecmp

### strncat

### strnchr

### strncmp

### strncpy

### strnicmp

### strnlen

### strnstr

### strpbrk

Finds the first character in the string *str1* that matches any character specified in *str2*.

### strrchr

Searches for the last occurrence of the character c (an unsigned char) in the string pointed to by the argument *str*.

### strsep

### strspn

Calculates the length of the initial segment of *str1* which consists entirely of characters in *str2*.

### strstr

Finds the first occurrence of the entire string *needle* (not including the terminating null character) which appears in the string *haystack*.

## time

### asctime

Returns a pointer to a string which represents the day and time of the structure timeptr.

### clock

Returns the processor clock time used since the beginning of an  implementation defined era (normally the beginning of the program).

### ctime

Returns a string representing the localtime based on the argument timer.

### difftime

Returns the difference of seconds between time1 and time2 (time1-time2).

### gettimeofday

### gmtime

The value of timer is broken up into the structure tm and expressed in  Coordinated Universal Time (UTC) also known as Greenwich Mean Time  (GMT).

### localtime

The value of timer is broken up into the structure tm and expressed in the local time zone.

### mktime

Converts the structure pointed to by timeptr into a time_t value according to the local time zone.

### strftime

Formats the time represented in the structure timeptr according to the formatting rules defined in format and stored into str.

### time

Calculates the current calender time and encodes it into time_t format.