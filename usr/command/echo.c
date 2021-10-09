
#include <limits.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>

/*
 * Report an error and exit.
 * Use it instead of err(3) to avoid linking-in stdio.
 */
static void error(const char * str, ...)
{
    va_list args;
    va_start(args, str);
    vfprintf(stderr, str, args);
    va_end(args);
    fputc('\n', stderr);
    exit(0);
}

int echo(int argc, char *argv[])
{
    int nflag;  /* if not set, output a trailing newline. */
    int veclen; /* number of writev arguments. */
    struct iovec *iov, *vp; /* Elements to write, current element. */
    char space[] = " ";
    char newline[] = "\n";
    char *progname = argv[0];

    /* This utility may NOT do getopt(3) option parsing. */
    if (*++argv && !strcmp(*argv, "-n")) {
        ++argv;
        --argc;
        nflag = 1;
    } else
        nflag = 0;

    veclen = (argc >= 2) ? (argc - 2) * 2 + 1 : 0;

    if ((vp = iov = malloc((veclen + 1) * sizeof(struct iovec))) == NULL)
        error("%s: malloc error", progname);

    while (argv[0] != NULL) {
        size_t len;

        len = strlen(argv[0]);

        /*
        * If the next argument is NULL then this is the last argument,
        * therefore we need to check for a trailing \c.
        */
        if (argv[1] == NULL) {
            /* is there room for a '\c' and is there one? */
            if (len >= 2 &&
                argv[0][len - 2] == '\\' &&
                argv[0][len - 1] == 'c') {
                /* chop it and set the no-newline flag. */
                len -= 2;
                nflag = 1;
            }
        }
        vp->iov_base = *argv;
        vp++->iov_len = len;
        if (*++argv) {
            vp->iov_base = space;
            vp++->iov_len = 1;
        }
    }
    if (!nflag) {
        veclen++;
        vp->iov_base = newline;
        vp++->iov_len = 1;
    }
    /* assert(veclen == (vp - iov)); */
    while (veclen) {
        int nwrite;

        nwrite = (veclen > INT_MAX) ? INT_MAX : veclen;
        if (writev(STDOUT_FILENO, iov, nwrite) == -1)
            error("%s: write error", progname);
        iov += nwrite;
        veclen -= nwrite;
    }
    return 0;
} 
