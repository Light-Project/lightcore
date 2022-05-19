/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <fwnode.h>
#include <driver/led.h>

enum led_default_state led_fwnode_default_state(struct fwnode *fwnode)
{
    const char *state = NULL;

    if (!fwnode_attribute_read_string(fwnode, "default-state", &state)) {
        if (!strcmp(state, "keep"))
            return LED_DEFSTATE_KEEP;
        if (!strcmp(state, "on"))
            return LED_DEFSTATE_ON;
    }

    return LED_DEFSTATE_OFF;
}