/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <kmalloc.h>
#include <driver/dt.h>
#include <printk.h>
#include <export.h>

state dt_parse_phandle_with_args_count(const struct dt_node *node, const char *list_name,
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
EXPORT_SYMBOL(dt_parse_phandle_with_args_count);

state dt_parse_phandle_with_args_map(const struct dt_node *node, const char *list_name,
                                     const char *stem_name, int index, struct dt_phandle_args *args)
{
    be32 initial_match_array[DT_PHANDLE_ARGS];
    const be32 *match_array = initial_match_array;
    char *cells_name, *map_name, *mask_name, *pass_name;
    const struct dt_node *current, *new;
    uint32_t list_size, count;
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

    retval = dt_parse_phandle_with_args_count(node, list_name, cells_name, -1, index, args);
    if (retval)
        goto pass_free;

    retval = dt_attribute_read_u32(args->node, cells_name, &list_size);
    if (retval)
        goto pass_free;

    for (count = 0; count < list_size; ++count)
        initial_match_array[count] = cpu_to_be32(args->args[count]);

    for (current = args->node; current; args->node = current = new) {
        const be32 mask_dummy[] = {[0 ... DT_PHANDLE_ARGS] = ~0};
        const be32 pass_dummy[] = {[0 ... DT_PHANDLE_ARGS] = ~0};
        uint32_t new_size;
        const be32 *map, *mask, *pass;
        int match, maplen;

        map = dt_attribute_get(current, map_name, &maplen);
        if (!map) {
            retval = -ENOERR;
            goto pass_free;
        }

        mask = dt_attribute_get(current, mask_name, NULL);
        if (!mask)
            mask = mask_dummy;

        for (match = 0, maplen /= sizeof(be32), retval = -EINVAL;
             maplen > (list_size + 1) && !match && (match = 1);
             maplen -= new_size, map += new_size) {

            /* Compare specifiers */
            for (count = 0; count < list_size; ++count, --maplen)
                match &= !((match_array[count] ^ *map++) & mask[count]);

            /* Get the entry parent */
            new = dt_find_by_phandle(be32_to_cpup(map));
            if (!new) {
                pr_warn("map %p parent not found\n", map);
                goto pass_free;
            }

            map++;
            maplen--;

            /* Check the parent status */
            if (!dt_node_check_available(new))
                match = 0;

            /* Get #<list>-cells for new parent */
            if (dt_attribute_read_u32(new, cells_name, &new_size)) {
                pr_warn("parent %p lacks %s\n", new, cells_name);
                goto pass_free;
            }

            /* Check for malformed properties */
            if ((new_size > DT_PHANDLE_ARGS) || (new_size > maplen)) {
                pr_warn("%p malformed properties\n", new);
                retval = -EOVERFLOW;
                goto pass_free;
            }
        }

        if (!match)
            goto pass_free;

        pass = dt_attribute_get(current, pass_name, NULL);
        if (!pass)
            pass = pass_dummy;

        for (count = 0; count < new_size; ++count) {
            be32 value = *(map - new_size + count);

            if (count < list_size) {
                value |= cpu_to_be32(args->args[count]);
                value &= ~pass[count];
            }

            args->args[count] = be32_to_cpu(value);
        }

        match_array = map - new_size;
    }

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
EXPORT_SYMBOL(dt_parse_phandle_with_args_map);
