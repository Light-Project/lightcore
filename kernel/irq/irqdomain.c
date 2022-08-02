/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "irq-domain"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <irq.h>
#include <irqdomain.h>
#include <kmalloc.h>
#include <export.h>

static struct irq_domain *root_domain;
static struct kcache *irq_domain_cache;

static LIST_HEAD(domain_list);
static MUTEX_LOCK(domain_lock);

static long domain_revmap_find(struct rb_node *node, void *pdata)
{
    struct irq_desc *irq_desc = domain_to_irqdesc(node);
    return (irqnr_t)pdata - irq_desc->hwirq;
}

static state domain_translate(struct irq_domain *domain, struct irq_domain_fwspec *fwspec,
                              irqnr_t *hwirq, unsigned long irq_flags)
{
    struct irq_domain_ops *ops = domain->ops;

    if (ops->xlate)
        return ops->xlate(domain, fwspec->fwnode, fwspec->param_count, fwspec->params, hwirq);

    *hwirq = fwspec->params[0];
    return -ENOERR;
}

struct irq_desc *irq_domain_mapping(struct irq_domain *domain, irqnr_t hwirq, irqnr_t *virq)
{
    struct irq_desc *desc = NULL;
    struct rb_node *node;

    if (!domain)
        domain = root_domain;
    if (!domain)
        return NULL;

    mutex_lock(&domain_lock);
    node = rb_find(&domain->revmap, (void *)hwirq, domain_revmap_find);
    mutex_unlock(&domain_lock);

    if (likely(node)) {
        desc = domain_to_irqdesc(node);
        if (virq)
            *virq = desc->virq;
    }

    return desc;
}
EXPORT_SYMBOL(irq_domain_mapping);

struct irq_domain *irq_domain_fwspec_matching(struct irq_domain_fwspec *fwspec,
                                              enum irq_domain_token token)
{
    struct irq_domain *walk, *find = NULL;
	struct fwnode *fwnode = fwspec->fwnode;

    mutex_lock(&domain_lock);
    list_for_each_entry(walk, &domain_list, list) {
        struct irq_domain_ops *ops = walk->ops;
        bool matched;

        if (ops->match)
            matched = ops->match(walk, fwnode, token);
        else
            matched = fwnode && fwnode == walk->fwnode &&
                      (token == IRQDOMAIN_BUS_ANY || token == walk->bus_token);

        if (matched) {
            find = walk;
            break;
        }
    }
    mutex_unlock(&domain_lock);

    return find;
}
EXPORT_SYMBOL(irq_domain_fwspec_matching);

irqnr_t irq_domain_fwspec_mapping(struct irq_domain_fwspec *fwspec)
{
    struct irq_domain *domain;
    unsigned long irq_flags;
    irqnr_t hwirq;

    if (!fwspec->fwnode)
        domain = root_domain;
    else {
        domain = irq_domain_fwspec_matching(fwspec, IRQDOMAIN_BUS_WIRED);
        if (!domain)
            domain = irq_domain_fwspec_matching(fwspec, IRQDOMAIN_BUS_ANY);
    }

    if (unlikely(!domain)) {
        pr_warn("failed to find domain for %s\n", fwnode_node_name(fwspec->fwnode));
        return 0;
    }

    if (irq_domain_translate(domain, fwspec, &hwirq, irq_flags))
        return 0;

    if (irq_flags & ~IRQ_TRIG_MASK)
        irq_flags &= ~IRQ_TRIG_MASK;




}
EXPORT_SYMBOL(irq_domain_fwspec_mapping);

struct irq_domain *irq_domain_register(irqnr_t size, irqnr_t hwmax)
{
    struct irq_domain *domain;

    domain = kcache_zalloc(irq_domain_cache, GFP_KERNEL);
    if (unlikely(!domain))
        return NULL;

    mutex_lock(&domain_lock);
    list_add(&domain_list, &domain->list);
    mutex_unlock(&domain_lock);

    return domain;
}
EXPORT_SYMBOL(irq_domain_register);

void irq_domain_unregister(struct irq_domain *domain)
{
    mutex_lock(&domain_lock);
    list_del(&domain->list);
    mutex_unlock(&domain_lock);
}
EXPORT_SYMBOL(irq_domain_unregister);

void __init irq_domain_init(void)
{
    irq_domain_cache = KCACHE_CREATE(struct irq_domain, KCACHE_PANIC);
}
