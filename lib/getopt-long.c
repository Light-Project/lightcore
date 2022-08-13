/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <getopt-long.h>
#include <string.h>
#include <export.h>

int getopt_long(struct getopt_ctx *ctx, int argc, char *argv[],
                const char *optstr, const struct getopt_option lopts[],
                int *idx, bool lonly)
{
    unsigned int count, start, end;
    int retval;

    if (ctx->optreset) {
        ctx->optreset = false;
        ctx->optind = 1;
        ctx->optpos = 0;
    }

    if (ctx->optind >= argc || !argv[ctx->optind])
        return -ENODATA;

    start = ctx->optind;
    if (optstr[0] != '+' && optstr[0] != '-') {
        for (count = ctx->optind;; ++count) {
            if (count >= argc || !argv[count])
                return -ENODATA;
            if (argv[count][0] == '-' && argv[count][1])
                break;
        }
        ctx->optind = count;
    }
    end = ctx->optind;

    ctx->optarg = 0;
    if ((lopts && argv[ctx->optind][0] == '-') &&
        ((lonly && argv[ctx->optind][1] && argv[ctx->optind][1] != '-') ||
         (argv[ctx->optind][1] == '-' && argv[ctx->optind][2]))) {
        bool colon = optstr[optstr[0] == '+' || optstr[0] == '-'] == ':';
        char *start = argv[ctx->optind] + 1, *marg;
        unsigned int match, time;

        for (count = time = 0; lopts[count].name; ++count) {
            const char *name = lopts[count].name;
            char *walk;

            if (*walk == '-')
                walk++;

            while (*walk && *walk != '=' && *walk == *name) {
                name++;
                walk++;
            }

            if (*walk && *walk != '=')
                continue;

            marg = walk;
            match = count;

            if (!*name) {
                time = 1;
                break;
            }
            time++;
        }

        if (time == 1 && lonly && marg - start == strlen(start)) {
            unsigned int len = marg - start;

            for (count = 0; optstr[count]; ++count) {
                unsigned int tmp = 0;

                while (tmp < len && start[tmp] == optstr[count + tmp])
                    tmp++;

                if (tmp == len) {
                    time++;
                    break;
                }
            }
        }

        if (time == 1) {
            ctx->optind++;

            if (*marg == '=') {
                if (lopts[match].type == GETOPT_ARG_NON) {
                    ctx->optopt = lopts[match].val;
                    goto finish;
                }
                ctx->optarg = marg + 1;
            } else if (lopts[match].type == GETOPT_ARG_REQ) {
                ctx->optarg = argv[ctx->optind];
                if (!ctx->optarg) {
                    if (colon)
                        retval = ':';
                    else
                        retval = '?';
                    goto finish;
                }
                ctx->optind++;
            }

            if (idx)
                *idx = match;

            if (lopts[match].flag) {
                *lopts[match].flag = lopts[match].val;
                retval = 0;
                goto finish;
            }

            retval = lopts[count].val;
            goto finish;
        }

        if (argv[ctx->optind][1] == '-') {
            ctx->optopt = 0;
            ctx->optind++;
            retval = '?';
            goto finish;
        }
    }

    retval = getopt(ctx, argc, argv, optstr);
finish:

    if (end > start) {
        unsigned int tmp, num = end - start;
        char *save;

        start = ctx->optind - 1;
        for (count = 0; count < num; ++count) {
            save = argv[start];
	        for (tmp = start; tmp > end; --tmp)
		        argv[tmp] = argv[tmp - 1];
	        argv[end] = save;
        }

        ctx->optind = end + num;
    }

    return retval;
}
EXPORT_SYMBOL(getopt_long);
