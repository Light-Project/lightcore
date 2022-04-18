/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <driver/dt.h>
#include <export.h>

state dt_parser_phandle_with_args_count(const struct dt_node *node, const char *list_name,
                                        const char *cells_name, int cell_count,
                                        int index, struct dt_phandle_args *args)
{
    struct dt_phandle_iterator iter;
    unsigned int current = 0;
    state retval;

    if (index < 0)
        return -EINVAL;

    dt_for_each_phandle(&iter, node, list_name, cells_name, cell_count, retval) {
        if (current++ != index)
            continue;

        if (!iter.phandle)
            return -ENOENT;

        if (args) {
            current = dt_iterative_phandle_args(&iter, args->args, DT_PHANDLE_ARGS);
            args->args_count = current;
            args->node = iter.node;
        }

        break;
    }

    return retval;
}
EXPORT_SYMBOL(dt_parser_phandle_with_args_count);

state dt_parser_phandle_with_args_map(const struct dt_node *node, const char *list_name,
                                      const char *stem_name, int index, struct dt_phandle_args *args)
{
    char *cells_name, *map_name, *mask_name, *pass_name;
    const struct dt_node *current;
    uint32_t list_size;
    state retval = -ENOMEM;

    if (index < 0)
        return -EINVAL;

    cells_name = kasprintf(GFP_KERNEL, "#%s-cells", stem_name);
    if (unlikely(!cells_name))
        return -ENOMEM;

    map_name = kasprintf(GFP_KERNEL, "#%s-map", stem_name);
    if (unlikely(!map_name))
        goto cells_free;

    mask_name = kasprintf(GFP_KERNEL, "#%s-map-mask", stem_name);
    if (unlikely(!mask_name))
        goto map_free;

    pass_name = kasprintf(GFP_KERNEL, "#%s-map-pass-thru", stem_name);
    if (unlikely(!pass_name))
        goto mask_free;

    retval = dt_parser_phandle_with_args_count(node, list_name, cells_name, -1, index, args);
    if (retval)
        goto pass_free;

    current = args->node;
    retval = dt_attribute_read_u32(current, cells_name, &list_size);
    if (retval)
        goto pass_free;


pass_free:
    kfree(pass_name);
mask_free:
    kfree(mask_name);
map_free:
    kfree(map_name);
cells_free:
    kfree(cells_name);
    return retval;
}
EXPORT_SYMBOL(dt_parser_phandle_with_args_map);
