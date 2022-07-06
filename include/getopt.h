/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _GETOPT_H_
#define _GETOPT_H_

#include <types.h>
#include <stddef.h>
#include <state.h>

struct getopt_ctx {
    char *optarg;
    char optopt;
    int optpos;
    int optind;
    bool optreset;
};

#define GETOPT_INIT (struct getopt_ctx){.optreset = true}
extern int getopt(struct getopt_ctx *ctx, int argc, char *argv[], const char *optstr);

#endif  /* _GETOPT_H_ */
