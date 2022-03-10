/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IRQFLAGS_H_
#define _IRQFLAGS_H_

#include <asm/irq.h>

#define irq_local_disabled()        ({ arch_irq_disabled();     })
#define irq_local_enable()          ({ arch_irq_enable();       })
#define irq_local_disable()         ({ arch_irq_disable();      })
#define irq_local_save()            ({ arch_irq_save();         })
#define irq_local_restore(flags)    ({ arch_irq_restore(flags); })

#endif  /* _IRQFLAGS_H_ */
