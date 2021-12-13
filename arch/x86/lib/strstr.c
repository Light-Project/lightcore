/* SPDX-License-Identifier: GPL-2.0 */
#include <string.h>
#include <export.h>

char *strstr(const char *cs, const char *ct)
{
    register char *res;
    int	d0, d1;

    asm volatile (
        "movl %6,%%edi\n"
        "repne\n"
        "scasb\n"
        "notl %%ecx\n"
        "decl %%ecx\n"  /* NOTE! This also sets Z if searchstring='' */
        "movl %%ecx,%%edx\n"
        "1:\n"
        "movl %6,%%edi\n"
        "movl %%esi,%%eax\n"
        "movl %%edx,%%ecx\n"
        "repe\n"
        "cmpsb\n"
        "je 2f\n"       /* also works for empty string, see above */
        "xchgl %%eax,%%esi\n"
        "incl %%esi\n"
        "cmpb $0,-1(%%eax)\n"
        "jne 1b\n"
        "xorl %%eax,%%eax\n"
        "2:"
        :"=a"(res), "=&c"(d0), "=&S"(d1)
        :"0"(0), "1"(0xffffffff), "2"(cs), "g"(ct)
        :"dx", "di"
    );

    return res;
}
EXPORT_SYMBOL(strstr);
