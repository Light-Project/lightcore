/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IRQDOMAIN_H_
#define _IRQDOMAIN_H_

#include <list.h>
#include <rbtree.h>
#include <mutex.h>
#include <fwnode.h>

#define IRQDOMAIN_FWSPEC_PARAMS 16

enum irq_domain_token {
    IRQDOMAIN_BUS_ANY       = 0,
    IRQDOMAIN_BUS_WIRED     = 1,
    IRQDOMAIN_BUS_PCI_MSI   = 2,
};

/**
 * struct irq_domain_fwspec -
 * @fwnode:
 * @param_count:
 * @params:
 */
struct irq_domain_fwspec {
    struct fwnode *fwnode;
    unsigned int param_count;
    uint32_t params[IRQDOMAIN_FWSPEC_PARAMS];
};

/**
 * struct irq_domain -
 * @list:
 * @name:
 * @ops:
 * @revmap:
 * @revlock:
 */
struct irq_domain {
    struct list_head list;
    const char *name;
    struct irq_domain_ops *ops;
    unsigned long flags;
    void *pdata;

    enum irq_domain_token bus_token;
    struct fwnode *fwnode;
    struct mutex revlock;
    struct rb_root revmap;
};

/**
 * struct irq_domain_ops -
 *
 *
 */
struct irq_domain_ops {
    bool (*match)(struct irq_domain *domain, struct fwnode *fwnode, enum irq_domain_token token);
    state (*xlate)(struct irq_domain *domain, struct fwnode *fwnode, uint32_t *param, unsigned int count, irqnr_t *hwirq);
};

extern struct irq_desc *irq_domain_mapping(struct irq_domain *domain, irqnr_t hwirq, irqnr_t *virq);
extern struct irq_domain *irq_domain_fwspec_matching(struct irq_domain_fwspec *fwspec, enum irq_domain_token token);
extern irqnr_t irq_domain_fwspec_mapping(struct irq_domain_fwspec *fwspec);
extern struct irq_domain *irq_domain_register(irqnr_t size, irqnr_t hwmax);
extern void irq_domain_unregister(struct irq_domain *domain);

#endif /* _IRQDOMAIN_H_ */
