/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <driver/firmware/dmi.h>
#include <export.h>

size_t dmi_region[DMI_REG_MAX];

const char *dmi_string_index(const struct dmi_header *dhead, int index)
{
    const char *str;

    if (!index)
        return NULL;

    for (str = (char *)dhead + dhead->length; --index;)
        str += strlen(str) + 1;

    while (*str == ' ')
        str++;

    return str;
}
EXPORT_SYMBOL(dmi_string_index);

void dmi_save_ident(const struct dmi_header *dhead, enum dmi_region_type region, int index)
{
    const uint8_t *data = (const uint8_t *)dhead;
    const char *str;

    if (dmi_region[region] || dhead->length <= index)
        return;

    str = dmi_string_index(dhead, data[index]);
    dmi_region[region] = (size_t)str;
}
EXPORT_SYMBOL(dmi_save_ident);

void dmi_save_release(const struct dmi_header *dhead, enum dmi_region_type region, int index)
{
    const uint8_t *minor, *major;

    if (!dhead || dhead->length <= index)
        return;

    minor = (uint8_t *)dhead + index;
    major = (uint8_t *)dhead + index - 1;
    dmi_region[region] = *major << 8 | *minor;
}
EXPORT_SYMBOL(dmi_save_release);

const char *dmi_get_ident(enum dmi_region_type region)
{
    return (const char *)dmi_region[region];
}
EXPORT_SYMBOL(dmi_get_ident);

uint16_t dmi_get_release(enum dmi_region_type region)
{
    return (uint16_t)dmi_region[region];
}
EXPORT_SYMBOL(dmi_get_release);

static bool dmi_match_one(struct dmi_device_id *dmi)
{
    unsigned int count;

    for (count = 0; count < ARRAY_SIZE(dmi->matches); ++count) {
        enum dmi_region_type type = dmi->matches[count].reg;
        const char *substr = dmi_get_ident(type);

        if (substr) {
            if (dmi->matches[count].exact_match) {
                if (!strcmp(substr, dmi->matches[count].substr))
                    continue;
            } else {
                if (strstr(substr, dmi->matches[count].substr))
                    continue;
            }
        }

        return false;
    }

    return true;
}

const void *dmi_system_check(struct dmi_device_id *dmi)
{
    while (*dmi->name) {
        if (dmi_match_one(dmi))
            return dmi->data;
        ++dmi;
    }

    return NULL;
}
