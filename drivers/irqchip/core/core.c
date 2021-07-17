/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <system/mutex.h>
#include <driver/irqchip.h>

static MUTEX_LOCK(irqchip_lock);

state irqchip_pass(irqnr_t vector)
{
    return -ENOERR;
}

state irqchip_register(struct irqchip_device *idev)
{

    if(!idev || !idev->ops)
        return -EINVAL;

    mutex_lock(&irqchip_lock);

    mutex_unlock(&irqchip_lock);

    return -ENOERR;
}
