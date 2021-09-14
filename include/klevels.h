/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KLEVELS_H_
#define _KLEVELS_H_

#define KERN_SOH        "\001"          /* ASCII Start Of Header */
#define KERN_SOH_ASCII  '\001'

#define KERN_EMERG      KERN_SOH "0"    /* system is unusable */
#define KERN_ALERT      KERN_SOH "1"    /* action must be taken immediately */
#define KERN_CRIT       KERN_SOH "2"    /* critical conditions */
#define KERN_ERR        KERN_SOH "3"    /* error conditions */
#define KERN_WARNING    KERN_SOH "4"    /* warning conditions */
#define KERN_NOTICE     KERN_SOH "5"    /* normal but significant condition */
#define KERN_INFO       KERN_SOH "6"    /* informational */
#define KERN_DEBUG      KERN_SOH "7"    /* debug-level messages */
#define KERN_DEFAULT    ""              /* the default kernel loglevel */

enum klevel{
    KLEVEL_EMERG        = 0,
    KLEVEL_ALERT        = 1,
    KLEVEL_CRIT         = 2,
    KLEVEL_ERR          = 3,
    KLEVEL_WARNING      = 4,
    KLEVEL_NOTICE       = 5,
    KLEVEL_INFO         = 6,
    KLEVEL_DEBUG        = 7,
    KLEVEL_DEFAULT      = 255,
};

#endif
