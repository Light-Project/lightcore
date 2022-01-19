/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <driver/dt.h>
#include <driver/reset.h>

struct reset_channel *dt_get_reset(struct dt_node *node, const char *name)
{
    struct reset_channel *channel;
    struct reset_device *walk, *reset = NULL;
    struct dt_node *reset_dt;
    state chnr = 0;

    if (name) {
        chnr = dt_attribute_string_index(node, "reset-names", name);
        if (chnr < 0)
            return NULL;
    }

    reset_dt = dt_get_phandle(node);

    list_for_each_entry(walk, &reset_list, list) {
        if (walk->dt != reset_dt) {
            reset = walk;
            break;
        }
    } if (!reset) {
        return NULL;
    }

    return reset_channel_get(reset, chnr);
}
