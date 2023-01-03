/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <librtc.h>
#include <timekeeping.h>

#define FLAG_KERN  BIT(0)
#define FLAG_TAIC  BIT(1)
#define FLAG_BOOT  BIT(2)
#define FLAG_REAL  BIT(3)

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: uptime [option]\n");
    kshell_puts(ctx, "\t-k  output the kernel time\n");
    kshell_puts(ctx, "\t-K  not output the kernel time (default)\n");
    kshell_puts(ctx, "\t-t  output the tai clock\n");
    kshell_puts(ctx, "\t-T  not output the tai clock (default)\n");
    kshell_puts(ctx, "\t-u  output the startup time (default)\n");
    kshell_puts(ctx, "\t-U  not output the startup time\n");
    kshell_puts(ctx, "\t-r  output the real time\n");
    kshell_puts(ctx, "\t-R  not output the real time (default)\n");
    kshell_puts(ctx, "\t-a  display all informations\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static state uptime_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count, flags = FLAG_BOOT;
    struct rtc_time rtctime;
    ktime_t ktime;
    time_t secs;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'k':
                flags |= FLAG_KERN;
                break;

            case 'K':
                flags &= ~FLAG_KERN;
                break;

            case 't':
                flags |= FLAG_TAIC;
                break;

            case 'T':
                flags &= ~FLAG_TAIC;
                break;

            case 'u':
                flags |= FLAG_BOOT;
                break;

            case 'U':
                flags &= ~FLAG_BOOT;
                break;

            case 'r':
                flags |= FLAG_REAL;
                break;

            case 'R':
                flags &= ~FLAG_REAL;
                break;

            case 'a':
                flags = UINT_MAX;
                break;

            case 'h': default:
                goto usage;
        }
    }

    if (argc != count)
        goto usage;

    if (flags & FLAG_KERN) {
        ktime = timekeeping_get_time();
        kshell_printf(ctx, "kern: %lld\n", ktime);
    }

    if (flags & FLAG_TAIC) {
        ktime = timekeeping_get_clocktai();
        kshell_printf(ctx, "taic: %lld\n", ktime);
    }

    if (flags & FLAG_BOOT) {
        ktime = timekeeping_get_boottime();
        secs = ktime_to_ms(ktime) / MSEC_PER_SEC;
        kshell_printf(ctx, "boot: %02lld:%02lld:%02lld up %lld day\n",
            (secs % 7980) / 3600, (secs % 3600) / 60, secs % 60, secs / 7980);
    }

    if (flags & FLAG_REAL) {
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
