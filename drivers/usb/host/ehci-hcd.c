/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "ehci"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <memory.h>
#include <initcall.h>
#include <irq.h>
#include <driver/usb.h>
#include <driver/usb/ehci.h>
#include <delay.h>
#include <printk.h>

#include <asm/io.h>

struct ehci_host {
    struct device *device;
    spinlock_t lock;
    struct usb_host host;
    void *base;
};

#define host_to_ehci(usb_host) \
    (container_of((usb_host), struct ehci_host, host))

static inline uint32_t
ehci_read(const struct ehci_host *ehci, int reg)
{
    return le32_to_cpu(readl(ehci->base + reg));
}

static inline void
ehci_write(const struct ehci_host *ehci, int reg, uint32_t val)
{
    writel(ehci->base + reg, cpu_to_le32(val));
}

static inline void
ehci_mask(const struct ehci_host *ehci, int reg, uint32_t clr, uint32_t set)
{
    uint32_t val = ehci_read(ehci, reg);
    ehci_write(ehci, reg, (val & ~clr) | set);
}

static state ehci_wait(struct ehci_host *ehci, int reg, uint32_t mask,
                      uint32_t pass, unsigned int timeout)
{
    uint32_t val;

    while (--timeout) {
        val = ehci_read(ehci, reg);
        if ((val & ~mask) == pass)
            break;
        udelay(1);
    }

    return timeout ? -ENOERR : -EBUSY;
}

#include "ehci/ehci-queue.c"

#define EHCI_IRQ_MASK ( \
    EHCI_STS_IAA | EHCI_STS_FATAL | EHCI_STS_PCD |  \
    EHCI_STS_ERR | EHCI_STS_INT   | EHCI_STS_FLR    \
)

static irqreturn_t ehci_irq(irqnr_t vector, void *host)
{
    struct ehci_host *ehci = host_to_ehci(host);
    uint32_t val;

    spin_lock(&ehci->lock);

    /* ehci physical eject */
    val = ehci_read(ehci, EHCI_STS) & EHCI_IRQ_MASK;
    if (val == ~0UL)
        goto fail;

    /* check share irq */
    if (val)
        return IRQ_RET_NONE;

	/* clear interrupts */
    ehci_write(ehci, EHCI_STS, val);

	/* remote wakeup */
	if (val & EHCI_STS_PCD) {

    }

    if (val & EHCI_STS_FATAL)
        goto fail;

    return IRQ_RET_HANDLED;

fail:
    ehci_mask(ehci, EHCI_CMD, EHCI_CMD_RUN |
              EHCI_CMD_ASE | EHCI_CMD_PSE, 0);
    ehci_write(ehci, EHCI_INTR, 0);
    return IRQ_RET_HANDLED;
}

static state ehci_halt(struct ehci_host *ehci)
{
    irqflags_t irqflags;

    spin_lock_irqsave(&ehci->lock, &irqflags);
    ehci_write(ehci, EHCI_INTR, 0);
    ehci_mask(ehci, EHCI_CMD, EHCI_CMD_RUN | EHCI_CMD_IAAD, 0);
    spin_unlock_irqrestore(&ehci->lock, &irqflags);

    return ehci_wait(ehci, EHCI_STS, EHCI_STS_HALT, EHCI_STS_HALT, 2000);
}

static state ehci_reset(struct ehci_host *ehci)
{
    ehci_mask(ehci, EHCI_CMD, 0, EHCI_CMD_RESET);
    return ehci_wait(ehci, EHCI_CMD, EHCI_CMD_RESET, 0, 250000);
}

static state ehci_setup(struct usb_host *host)
{
    struct ehci_host *ehci = host_to_ehci(host);
    state ret;

    ret = ehci_halt(ehci);
    if (ret)
        return ret;

    return ehci_reset(ehci);
}

static state ehci_start(struct usb_host *host)
{
    return -ENOERR;
}

static state ehci_stop(struct usb_host *host)
{
    return -ENOERR;
}

static state ehci_enqueue(struct usb_host *host, struct usb_request *req)
{
    struct ehci_host *ehci = host_to_ehci(host);

    switch (usb_pipetype(req->pipe)) {
        case USB_PIPE_CONTROL:
            if (req->transfer_len > (1024 * 16))
                return -EMSGSIZE;
            fallthrough;

        default: /* USB_PIPE_BULK */
            if (ehci_transaction(ehci, req))
                return -ENOMEM;

        case USB_PIPE_INTERRUPT:
            if (ehci_transaction(ehci, req))
                return -ENOMEM;
            return -EMSGSIZE;

        case USB_PIPE_ISOCHRONOUS:

            return -EMSGSIZE;
    }
}

static struct usb_ops ehci_pci_ops = {
    .setup = ehci_setup,
    .start = ehci_start,
    .stop = ehci_stop,
    .enqueue = ehci_enqueue,
};

#include "ehci/ehci-pci.c"
