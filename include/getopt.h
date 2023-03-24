/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _GETOPT_H_
#define _GETOPT_H_

#include <types.h>
#include <stddef.h>
#include <state.h>

/**
 * struct getopt_ctx - describes context of param acquisition.
 * @optind: index in the argument value array.
 * @optarg: string value of the argument to an option.
 * @optopt: letter representing the command-line option.
 */
struct getopt_ctx {
    int optind;
    char *optarg;
    char optopt;
    int optpos;
    bool optreset;
};

#define GETOPT_INIT (struct getopt_ctx){.optreset = true}
extern int getopt(struct getopt_ctx *ctx, int argc, char *argv[], const char *optstr);

#endif /* _GETOPT_H_ */
