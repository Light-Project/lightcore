/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <driver/firmware/dmi.h>

static size_t dmi_region[DMI_REG_MAX];

char *dmi_string_index(const struct dmi_header *dmi, int index)
{
    char *str;

    if (!index)
        return NULL;

    for (str = (char *)dmi + dmi->length; --index;)
        str += strlen(str) + 1;

    while (*str == ' ')
        str++;

    return str;
}

void dmi_region_byte(struct dmi_header *dmi, int index, enum dmi_region_type region)
{
    if (!dmi || dmi->length <=index)
        return;
    dmi_region[region] = *((uint8_t *)dmi + index);
}

void dmi_region_word(struct dmi_header *dmi, int index, enum dmi_region_type region)
{
    if (!dmi || dmi->length <=index)
        return;
    dmi_region[region] = *((uint16_t *)dmi + index);
}

void dmi_region_string(struct dmi_header *dmi, int index, enum dmi_region_type region)
{
    if (!dmi || dmi->length <=index)
        return;
    dmi_region[region] = (size_t)dmi_string_index(dmi, index);
}

bool dmi_match_one(enum dmi_region_type type, const char *name)
{
    char *region = (char *)dmi_region[type];

    if (!region || !name)
        return region == name;

    return !strcmp(region, name);
}

void *dmi_match(struct dmi_deivce_id *dmi)
{
    while (dmi->name[0]) {
        if (dmi_match_one(dmi->region, dmi->name))
            return dmi->data;
        ++dmi;
    }

    return NULL;
}