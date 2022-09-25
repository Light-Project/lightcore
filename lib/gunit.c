/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <gunit.h>
#include <kernel.h>
#include <export.h>

int gunit(char *buff, uintmax_t value, unsigned int step,
          const char *unit[], unsigned int units, size_t bsize)
{
    unsigned int count = 0;
    uintmax_t prev = 0;

    while ((value >= step) && (count <= units)) {
        value = (prev = value) / step;
        ++count;
    }

    return scnprintf(
        buff, bsize, "%u.%02u%s", (uint32_t)value,
        (uint32_t)(prev % step) / (step / 100), unit[count]
    );
}
EXPORT_SYMBOL(gunit);

static const char *gsize_unit[7] = {
    "Byte",
    "KiB", "MiB", "GiB",
    "TiB", "PiB", "EiB",
};

int gsize(char *buff, uintmax_t value)
{
    return gunit(buff, value, 1024, gsize_unit,
                 ARRAY_SIZE(gsize_unit), GSIZE_BUFF);
}
EXPORT_SYMBOL(gsize);

static const char *gfreq_unit[4] = {
    "Hz",
    "KHz", "MHz", "GHz",
};

int gfreq(char *buff, uintmax_t value)
{
    return gunit(buff, value, 1000, gfreq_unit,
                 ARRAY_SIZE(gfreq_unit), GFREQ_BUFF);
}
EXPORT_SYMBOL(gfreq);
