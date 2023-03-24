/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IRQTYPES_H_
#define _IRQTYPES_H_

#include <bitmap.h>
#include <percpu.h>
#include <asm/irq.h>

#ifndef ARCH_IRQSTATE
typedef struct {} arch_irqstat_t;
#endif

struct irq_cpustat {
    DEFINE_BITMAP(softirq_pending, CONFIG_SOFTIRQ_LIMIT);
    unsigned long nmi_count;
    arch_irqstat_t arch;
};

DECLARE_PERCPU(struct irq_cpustat, irq_stat);

#endif /* _IRQTYPES_H_ */
