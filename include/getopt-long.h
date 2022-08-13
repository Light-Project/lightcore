/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _GETOPT_LONG_H_
#define _GETOPT_LONG_H_

#include <types.h>
#include <getopt.h>

enum getopt_arg {
    GETOPT_ARG_NON  = 0,
    GETOPT_ARG_REQ  = 1,
    GETOPT_ARG_OPT  = 2,
};

struct getopt_option {
    const char *name;
    enum getopt_arg type;
    char *flag;
    char val;
};

extern int getopt_long(struct getopt_ctx *ctx, int argc, char *argv[],
                       const char *optstr, const struct getopt_option longopts[],
                       int *idx, bool lonly);

#endif  /* _GETOPT_LONG_H_ */
