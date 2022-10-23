/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KLEVELS_H_
#define _KLEVELS_H_

/* ASCII Start Of Header */
#define KERN_SOH                "\001"
#define KERN_SOH_ASCII          '\001'

#define KERN_EMERG      KERN_SOH "0"    /* system is unusable */
#define KERN_ALERT      KERN_SOH "1"    /* action must be taken immediately */
#define KERN_CRIT       KERN_SOH "2"    /* critical conditions */
#define KERN_ERR        KERN_SOH "3"    /* error conditions */
#define KERN_WARNING    KERN_SOH "4"    /* warning conditions */
#define KERN_NOTICE     KERN_SOH "5"    /* normal but significant condition */
#define KERN_INFO       KERN_SOH "6"    /* informational */
#define KERN_DEBUG      KERN_SOH "7"    /* debug-level messages */
#define KERN_DEFAULT    ""              /* the default kernel loglevel */

#define KLEVEL_EMERG            0
#define KLEVEL_ALERT            1
#define KLEVEL_CRIT             2
#define KLEVEL_ERR              3
#define KLEVEL_WARNING          4
#define KLEVEL_NOTICE           5
#define KLEVEL_INFO             6
#define KLEVEL_DEBUG            7
#define KLEVEL_DEFAULT          8

#define KERN_COLR_DEFAULT       39
#define KERN_COLR_BLACK         30
#define KERN_COLR_DARK_RED      31
#define KERN_COLR_DARK_GREEN    32
#define KERN_COLR_DARK_YELLOW   33
#define KERN_COLR_DARK_BLUE     34
#define KERN_COLR_DARK_MAGENTA  35
#define KERN_COLR_DARK_CYAN     36
#define KERN_COLR_LIGHT_GRAY    37
#define KERN_COLR_DARK_GRAY     90
#define KERN_COLR_RED           91
#define KERN_COLR_GREEN         92
#define KERN_COLR_YELLOW        93
#define KERN_COLR_BLUE          94
#define KERN_COLR_MAGENTA       95
#define KERN_COLR_CYAN          96
#define KERN_COLR_WHITE         97

#endif /* _KLEVELS_H_ */
