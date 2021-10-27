/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <string.h>
#include <kmalloc.h>
#include <ctype.h>
#include <export.h>

/*
 * String manipulation
 */
__weak char *strcpy(char *dest, const char *src)
{
    char *tmp = dest;

    while ((*dest++ = *src++) != '\0');
    return tmp;
}
EXPORT_SYMBOL(strcpy);

__weak char *strncpy(char *dest, const char *src, size_t n)
{
    char *tmp = dest;

    while (n)
    {
        if ((*tmp = *src) != 0)
            src++;
        tmp++;
        n--;
    }
    return dest;
}
EXPORT_SYMBOL(strncpy);

__weak size_t strlcpy(char *dest, const char *src, size_t n)
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
EXPORT_SYMBOL(strlcpy);

__weak char *strcat(char *dest, const char *src)
{
    char *tmp = dest;

    while (*dest)
        dest++;
    while ((*dest++ = *src++) != '\0');

    return tmp;
}
EXPORT_SYMBOL(strcat);

__weak char *strncat(char *dest, const char *src, size_t n)
{
    char *tmp = dest;

    if (n)
    {
        while (*dest)
            dest++;
        while ((*dest++ = *src++) != 0)
        {
            if (--n == 0)
            {
                *dest = '\0';
                break;
            }
        }
    }

    return tmp;
}
EXPORT_SYMBOL(strncat);

__weak size_t strlcat(char *dest, const char *src, size_t n)
{
    size_t dsize = strlen(dest);
    size_t len = strlen(src);
    size_t res = dsize + len;

    dest += dsize;
    n -= dsize;

    if (len >= n)
        len = n-1;

    memcpy(dest, src, len);
    dest[len] = 0;

    return res;
}
EXPORT_SYMBOL(strlcat);

__weak int strcoll(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}
EXPORT_SYMBOL(strcoll);

/*
 * String examination
 */
__weak size_t strlen(const char *s)
{
    const char *len;
    for(len = s; *len != '\0'; len++);
    return len - s;
}
EXPORT_SYMBOL(strlen);

__weak size_t strnlen(const char *s, size_t n)
{
    const char *sc;

    for (sc = s; n-- && *sc != '\0'; ++sc);
    return sc - s;
}
EXPORT_SYMBOL(strnlen);

__weak int strcmp(const char *s1, const char *s2)
{
    int cp = 0;
    while((!(cp = *s1++ - *s2++)) && (*s1 != '\0'));
    return cp;
}
EXPORT_SYMBOL(strcmp);

__weak int strncmp(const char *s1, const char *s2, size_t n)
{
    int __res = 0;

    while (n)
    {
        if ((__res = *s1 - *s2++) != 0 || !*s1++)
            break;
        n--;
    }
    return __res;
}
EXPORT_SYMBOL(strncmp);

__weak int strnicmp(const char *s1, const char *s2, size_t n)
{
    unsigned char c1 = 0, c2 = 0;

    if (n) {
        do {
            c1 = *s1;
            c2 = *s2;
            s1++;
            s2++;
            if (!c1)
                break;
            if (!c2)
                break;
            if (c1 == c2)
                continue;
            c1 = tolower(c1);
            c2 = tolower(c2);
            if (c1 != c2)
                break;
        } while (--n);
    }
    return (int)c1 - (int)c2;
}
EXPORT_SYMBOL(strnicmp);

__weak int strcasecmp(const char *s1, const char *s2)
{
    int c1, c2;

    do {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while (c1 == c2 && c1 != 0);

    return c1 - c2;
}
EXPORT_SYMBOL(strcasecmp);

__weak int strncasecmp(const char *s1, const char *s2, size_t n)
{
    int c1, c2;

    do {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while ((--n > 0) && c1 == c2 && c1 != 0);

    return c1 - c2;
}
EXPORT_SYMBOL(strncasecmp);

__weak char *strchr(const char *s, int c)
{
    for (; *s != (char)c; ++s)
        if (*s == '\0')
            return NULL;
    return (char *)s;
}
EXPORT_SYMBOL(strchr);

__weak char *strrchr(const char *s, int c)
{
    const char *p = s + strlen(s);

    do {
    if (*p == (char)c)
        return (char *)p;
    } while (--p >= s);

    return NULL;
}
EXPORT_SYMBOL(strrchr);

__weak char *strnchr(const char *s, size_t n, int c)
{
    for (; n-- && *s != '\0'; ++s)
        if (*s == (char)c)
            return (char *)s;
    return NULL;
}
EXPORT_SYMBOL(strnchr);

char *strchrnul(const char *s, int c)
{
    while (*s && *s != (char)c)
        s++;
    return (char *)s;
}
EXPORT_SYMBOL(strchrnul);

char *strnchrnul(const char *s, size_t count, int c)
{
    while (count-- && *s && *s != (char)c)
        s++;
    return (char *)s;
}
EXPORT_SYMBOL(strnchrnul);


__weak size_t strspn(const char *s, const char *accept)
{
    const char *p;
    const char *a;
    size_t count = 0;

    for (p = s; *p != '\0'; ++p) {
        for (a = accept; *a != '\0'; ++a) {
            if (*p == *a)
                break;
        }
        if (*a == '\0')
            return count;
        ++count;
    }

    return count;
}
EXPORT_SYMBOL(strspn);

__weak size_t strcspn(const char *s, const char *reject)
{
    const char *p;
    const char *r;
    size_t count = 0;

    for (p = s; *p != '\0'; ++p) {
        for (r = reject; *r != '\0'; ++r) {
            if (*p == *r)
                return count;
        }
        ++count;
    }

    return count;
}
EXPORT_SYMBOL(strcspn);

__weak char *strpbrk(const char *s1, const char *s2)
{
    const char *sc1, *sc2;

    for (sc1 = s1; *sc1 != '\0'; ++sc1) {
        for (sc2 = s2; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *)sc1;
        }
    }

    return NULL;
}
EXPORT_SYMBOL(strpbrk);

__weak char *strstr(const char *s1, const char *s2)
{
    size_t l1, l2;

    l2 = strlen(s2);
    if (!l2)
        return (char *)s1;

    l1 = strlen(s1);
    while (l1 >= l2) {
        l1--;
        if (!memcmp(s1, s2, l2))
            return (char *)s1;
        s1++;
    }

    return NULL;
}
EXPORT_SYMBOL(strstr);

__weak char *strnstr(const char *s1, const char *s2, size_t n)
{
    size_t l2;

    l2 = strlen(s2);
    if (!l2)
        return (char *)s1;

    while (n >= l2)
    {
        n--;
        if (!memcmp(s1, s2, l2))
            return (char *)s1;
        s1++;
    }

    return NULL;
}
EXPORT_SYMBOL(strnstr);

__weak char *strdup(const char *s)
{
    char *p;

    if (!s)
        return NULL;

    p = kmalloc(strlen(s) + 1, GFP_KERNEL);
    if (p)
        return(strcpy(p, s));

    return NULL;
}
EXPORT_SYMBOL(strdup);

__weak char *strsep(char **s, const char *ct)
{
    char *sbegin = *s;
    char *end;

    if (sbegin == NULL)
        return NULL;

    end = strpbrk(sbegin, ct);
    if (end)
        *end++ = '\0';
    *s = end;

    return sbegin;
}
EXPORT_SYMBOL(strsep);

__weak char *strim(char *s)
{
    char *e;

    if (s) {
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
EXPORT_SYMBOL(strim);

/*
 * Memory manipulation
 */
__weak void *memset(void *s, int c, size_t n)
{
    char *xs = s;

    while (n--)
        *xs++ = c;

    return s;
}
EXPORT_SYMBOL(memset);

__weak void *memcpy(void *dest, const void*src, size_t len)
{
    uint8_t *ndest = (uint8_t *)dest;
    uint8_t *nsrc = (uint8_t *)src;

    if (ndest == NULL || nsrc == NULL || len == 0)
        return NULL;

    while (len--)
        *ndest++ = *nsrc++;

    return dest;
}
EXPORT_SYMBOL(memcpy);

__weak void *memmove(void *dest, const void *src, size_t n)
{
    char *tmp;
    const char *s;

    if (dest <= src) {
        tmp = dest;
        s = src;
        while (n--)
            *tmp++ = *s++;
    } else {
        tmp = dest;
        tmp += n;
        s = src;
        s += n;
        while (n--)
            *--tmp = *--s;
    }

    return dest;
}
EXPORT_SYMBOL(memmove);

__weak void *memchr(const void *s, int c, size_t n)
{
    char *str = (char *)s;

    while (n-- != 0)
        if (c == *str++)
            return str - 1;

    return NULL;
}
EXPORT_SYMBOL(memchr);

__weak void *memscan(void *addr, int c, size_t size)
{
    unsigned char *p = addr;

    while (size) {
        if (*p == c)
            return (void *)p;
        p++;
        size--;
    }

    return (void *)p;
}
EXPORT_SYMBOL(memscan);

__weak int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *su1, *su2;
    int res = 0;

    for (su1 = s1, su2 = s2; 0 < n; ++su1, ++su2, n--)
        if ((res = *su1 - *su2) != 0)
            break;

    return res;
}
EXPORT_SYMBOL(memcmp);

int memcount(const void *addr, int c, size_t size)
{
    const char *p = addr;
    int count;

    while (size--)
        if (c == *p++)
            count++;

    return count;
}
EXPORT_SYMBOL(memcount);

char *skip_spaces(const char *str)
{
	while (isspace(*str))
		++str;
	return (char *)str;
}
EXPORT_SYMBOL(skip_spaces);

char *basename(const char *path)
{
    const char *c = strrchr(path, '/');
    return (char *)(c ? c + 1 : path);
}
EXPORT_SYMBOL(basename);
