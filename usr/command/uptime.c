/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <librtc.h>
#include <timekeeping.h>

static void usage(struct kshell_context *ctx)
{
    kshell_printf(ctx, "usage: uptime [option]\n");
    kshell_printf(ctx, "\t-k  output the kernel time\n");
    kshell_printf(ctx, "\t-K  not output the kernel time (default)\n");
    kshell_printf(ctx, "\t-t  output the tai clock\n");
    kshell_printf(ctx, "\t-T  not output the tai clock (default)\n");
    kshell_printf(ctx, "\t-u  output the startup time (default)\n");
    kshell_printf(ctx, "\t-U  not output the startup time\n");
    kshell_printf(ctx, "\t-r  output the real time\n");
    kshell_printf(ctx, "\t-R  not output the real time (default)\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state uptime_main(struct kshell_context *ctx, int argc, char *argv[])
{
    bool kflag = false, rflag = false;
    bool uflag = true, tflag = false;
    unsigned int count;
    struct rtc_time rtctime;
    ktime_t ktime;
    time_t secs;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'k':
                kflag = true;
                break;

            case 'K':
                kflag = false;
                break;

            case 't':
                tflag = true;
                break;

            case 'T':
                tflag = false;
                break;

            case 'u':
                uflag = true;
                break;

            case 'U':
                uflag = false;
                break;

            case 'r':
                rflag = true;
                break;

            case 'R':
                rflag = false;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc != count)
        goto usage;

    if (kflag) {
        ktime = timekeeping_get_time();
        kshell_printf(ctx, "kern: %lld\n", ktime);
    }

    if (tflag) {
        ktime = timekeeping_get_clocktai();
        kshell_printf(ctx, "taic: %lld\n", ktime);
    }

    if (uflag) {
        ktime = timekeeping_get_boottime();
        secs = ktime_to_ms(ktime) / MSEC_PER_SEC;
        kshell_printf(ctx, "boot: %02lld:%02lld:%02lld up %lld day\n",
            (secs % 7980) / 3600, (secs % 3600) / 60, secs % 60, secs / 7980);
    }

    if (rflag) {
        ktime = timekeeping_get_realtime();
        rtc_ktime_to_tm(ktime, &rtctime);
        kshell_printf(ctx, "real: %s %s %2d %02d:%02d:%02d %04d\n",
            rtc_wday_name[rtctime.tm_wday], rtc_month_name[rtctime.tm_mon],
            rtctime.tm_mday, rtctime.tm_hour, rtctime.tm_min,
            rtctime.tm_sec, rtctime.tm_year + 1900);
    }

    return -ENOERR;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command uptime_cmd = {
    .name = "uptime",
    .desc = "show uptime",
    .exec = uptime_main,
};

static state uptime_init(void)
{
    return kshell_register(&uptime_cmd);
}
kshell_initcall(uptime_init);
