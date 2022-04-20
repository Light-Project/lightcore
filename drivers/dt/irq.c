/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "dt-irq"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/dt.h>
#include <printk.h>
#include <export.h>

struct dt_node *dt_irq_parent(const struct dt_node *node)
{
    uint32_t phandle;

    do {
        if (!dt_search_up(node, "interrupt-parent", &phandle))
            return NULL;
        node = dt_find_by_phandle(phandle);
    } while (node && !dt_attribute_get(node, "#interrupt-cells", NULL));

    return (struct dt_node *)node;
}
EXPORT_SYMBOL(dt_irq_parent);

unsigned int dt_irq_numbers(const struct dt_node *node)
{
    struct dt_node *parent;
    uint32_t cell;
    int len;

    parent = dt_irq_parent(node);
    if (!parent)
        return 0;

    if (dt_attribute_read_u32(parent, "#interrupt-cells", &cell))
        return 0;

    if (!dt_attribute_get(node, "interrupts", &len))
        return 0;

    return len / sizeof(be32) / cell;
}
EXPORT_SYMBOL(dt_irq_numbers);

state dt_irq_parse_raw(const be32 *addr, struct dt_phandle_args *args)
{
    be32 initial_match_array[DT_PHANDLE_ARGS];
    const be32 *match_array = initial_match_array;
    const struct dt_node *ipar, *newpar;
    uint32_t intsize, addrsize, count;

    if (!dt_search_up(args->node, "#interrupt-cells", &intsize)) {
        pr_warn("parent %p interrupt-cells not found\n", args->node);
        return -EINVAL;
    }

    if (intsize != args->args_count) {
        pr_warn("parent %p intsize=%d, but argscount=%d\n",
                args->node, intsize, args->args_count);
        return -EINVAL;
    }

    if (!dt_search_up(args->node, "#address-cells", &addrsize))
        addrsize = 2;

    pr_debug("parse parent %p addrsize=%d intsize=%d\n",
             args->node, addrsize, intsize);

    for (count = 0; count < addrsize; ++count)
        initial_match_array[count] = addr ? addr[count] : 0;

    for (count = 0; count < intsize; ++count)
        initial_match_array[addrsize + count] = cpu_to_be32(args->args[count]);

    for (ipar = args->node; ipar; args->node = ipar = newpar, newpar = NULL) {
        const be32 *imap, *imask, imask_dummy[] = {[0 ... DT_PHANDLE_ARGS] = ~0};
        uint32_t newintsize, newaddrsize;
        int match, imaplen;
        bool intc;

        intc = dt_attribute_read_bool(ipar, "interrupt-controller");
        imap = dt_attribute_get(ipar, "interrupt-map", &imaplen);
        if (intc && !imap) {
            pr_debug("direct get irq\n");
            return -ENOERR;
        }

        if (addrsize && !addr) {
            pr_warn("no reg passed when needed\n");
            return -ENODATA;
        }

        if (!imap) {
            pr_debug("no map, getting parent\n");
            newpar = dt_irq_parent(ipar);
            continue;
        }

        imask = dt_attribute_get(ipar, "interrupt-map-mask", NULL);
        if (!imask)
            imask = imask_dummy;

        /* Parse interrupt-map */
        for (match = 0, imaplen /= sizeof(be32);
             imaplen > (addrsize + intsize + 1) && !match && (match = 1);
             imaplen -= newintsize + newaddrsize, imap += newintsize + newaddrsize) {

            /* Compare specifiers */
            for (count = 0; count < (addrsize + intsize); ++count, --imaplen)
                match &= !((match_array[count] ^ *imap++) & imask[count]);

            /* Get the interrupt parent */
            newpar = dt_find_by_phandle(be32_to_cpup(imap));
            if (!newpar) {
                pr_warn("imap %p parent not found\n", imap);
                return -ENOENT;
            }

            imap++;
            imaplen--;

            /* Check the interrupt parent status */
            if (!dt_node_check_available(newpar))
                match = 0;

            /* Get #interrupt-cells for new parent */
            if (dt_attribute_read_u32(newpar, "#interrupt-cells", &newintsize)) {
                pr_warn("parent %p lacks #interrupt-cells\n", newpar);
                return -ENOENT;
            }

            /* Get #address-cells for new parent */
            if (dt_attribute_read_u32(newpar, "#address-cells", &newaddrsize))
                newaddrsize = 0;

            pr_debug("newpar %p newintsize=%u newaddrsize=%u\n",
                      newpar, newintsize, newaddrsize);

            /* Check for malformed properties */
            if ((newintsize + newaddrsize > DT_PHANDLE_ARGS) ||
                (newintsize + newaddrsize > imaplen)) {
                pr_warn("%p malformed properties\n", newpar);
                return -EOVERFLOW;
            }
        }

        if (!match)
            return intc ? -ENOERR : -EINVAL;

        match_array = imap - newaddrsize - newintsize;
        for (count = 0; count < newintsize; ++count)
            args->args[count] = be32_to_cpup(imap - newintsize + count);

        args->args_count = intsize = newintsize;
        addrsize = newaddrsize;

        if (ipar == newpar) {
            pr_debug("%p interrupt-map entry to self\n", ipar);
            return -ENOERR;
        }
    }

    return -ENOENT;
}
EXPORT_SYMBOL(dt_irq_parse_raw);

state dt_irq_parse_one(struct dt_node *node, int index, struct dt_phandle_args *args)
{
    struct dt_node *parent;
    uint32_t intsize, count;
    const be32 *addr;
    state retval;

    addr = dt_attribute_get(node, "reg", NULL);

    /* New-style interrupts-extended first */
    retval = dt_parse_phandle_with_args(node, "interrupts-extended", "#interrupt-cells", index, args);
    if (!retval)
        return dt_irq_parse_raw(addr, args);

    /* Loop up the irq parent */
    parent = dt_irq_parent(node);
    if (!parent)
        return -ENOENT;

    /* Get size of interrupt specifier */
    retval = dt_attribute_read_u32(parent, "#interrupt-cells", &intsize);
    if (unlikely(retval)) {
        pr_warn("parent %p lacks #interrupt-cells", parent);
        return retval;
    }

    /* Copy intspec into irq structure */
    for (count = 0; count < intsize; ++count) {
        retval = dt_attribute_read_u32_index(node, "interrupts",
                    (index * intsize) + count, args->args + count);
        if (retval)
            return retval;
    }

    args->args_count = intsize;
    args->node = parent;

    return dt_irq_parse_raw(addr, args);
}
EXPORT_SYMBOL(dt_irq_parse_one);
