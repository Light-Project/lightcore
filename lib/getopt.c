/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <getopt.h>
#include <export.h>

int getopt(struct getopt_ctx *ctx, int argc, char *argv[], const char *optstr)
{
    unsigned int count;
    char argch, optch;

    if (ctx->optreset) {
        ctx->optreset = false;
        ctx->optind = 1;
        ctx->optpos = 0;
    }

    if (ctx->optind >= argc || !argv[ctx->optind])
        return -ENODATA;

    if (argv[ctx->optind][0] != '-') {
        if (optstr[0] != '-')
            return -ENODATA;
        ctx->optarg = argv[ctx->optind++];
        return -ENOERR;
    }

    if (!argv[ctx->optind][1])
        return -EINVAL;

    if (argv[ctx->optind][1] == '-' && !argv[ctx->optind][2]) {
        ctx->optind++;
        return -EINVAL;
    }

    if (!ctx->optpos)
        ctx->optpos++;

    argch = argv[ctx->optind][ctx->optpos++];
    if (!argv[ctx->optind][ctx->optpos]) {
        ctx->optpos = 0;
        ctx->optind++;
    }

    if (optstr[0] == '-' || optstr[0] == '+')
        optstr++;

    for (count = 0; (optch = optstr[count]); ++count) {
        if (optch == argch)
            break;
    }

    if (!optch || optch == ':') {
        ctx->optopt = argch;
        return '?';
    }

    if (optstr[count + 1] == ':') {
        if (optstr[count + 2] == ':' && !ctx->optpos)
            ctx->optarg = NULL;
        else {
            ctx->optarg = argv[ctx->optind++] + ctx->optpos;
            ctx->optpos = 0;
        }
        if (ctx->optind > argc) {
            ctx->optopt = argch;
            if (optstr[0] == ':')
                return ':';
            return '?';
        }
    }

    return argch;
}
EXPORT_SYMBOL(getopt);
