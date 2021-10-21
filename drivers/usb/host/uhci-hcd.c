/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "uhci"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <kmalloc.h>
#include <initcall.h>
#include <irq.h>
#include <driver/usb.h>
#include <driver/usb/uhci.h>
#include <printk.h>

#include <asm/io.h>

struct uhci_host {
    struct device *device;

    spinlock_t lock;
    struct usb_host host;
    resource_size_t *mmio;
    resource_size_t  port;
};

#define host_to_uhci(usb_host) \
    (container_of((usb_host), struct uhci_host, host))

static inline uint8_t
uhci_readb(const struct uhci_host *host, int reg)
{
    if(host->mmio)
        return readb(host->mmio + reg);
    return inb(host->port + reg);
}

static inline uint16_t
uhci_readw(const struct uhci_host *host, int reg)
{
    if(host->mmio)
        return readw(host->mmio + reg);
    return inw(host->port + reg);
}

static inline uint32_t
uhci_readl(const struct uhci_host *host, int reg)
{
    if(host->mmio)
        return readl(host->mmio + reg);
    return inl(host->port + reg);
}

static inline void
uhci_writeb(const struct uhci_host *host, int reg, uint8_t val)
{
    if(host->mmio)
        return writeb(host->mmio + reg, val);
    outb(host->port + reg, val);
}

static inline void
uhci_writew(const struct uhci_host *host, int reg, uint16_t val)
{
    if(host->mmio)
        return writew(host->mmio + reg, val);
    outw(host->port + reg, val);
}

static inline void
uhci_writel(const struct uhci_host *host, int reg, uint32_t val)
{
    if(host->mmio)
        return writel(host->mmio + reg, val);
    outl(host->port + reg, val);
}

static inline int uhci_submit_control()
{

    return 0;
}

static state uhci_enqueue(struct usb_host *host,
                          struct usb_request *request)
{
    struct uhci_host *uhci = host_to_uhci(host);
    irqflags_t irqflags;

    spin_lock_irqsave(&uhci->lock, &irqflags);



    spin_unlock_irqrestore(&uhci->lock, &irqflags);
    return -ENOERR;
}

static irqreturn_t uhci_handle(irqnr_t vector, void *data)
{
    struct uhci_host *uhci = data;
    uint16_t val;

    val = uhci_readw(uhci, UHCI_STS);
    if (!(val & ~UHCI_STS_HCH))
        return IRQ_RET_NONE;
    uhci_writew(uhci, UHCI_STS, val);   /* Clear irq */

    spin_lock(&uhci->lock);

    if (val & ~(UHCI_STS_USBINT | UHCI_STS_ERROR | UHCI_STS_RD)) {
        if (val & UHCI_STS_HSE)
            pr_err("\n");
        if (val & UHCI_STS_HCPE)
            pr_err("\n");
        if (val & UHCI_STS_HCH)
            pr_err("\n");
    }

    if (val & UHCI_STS_RD) {

    } else {

    }

    spin_unlock(&uhci->lock);
    return IRQ_RET_HANDLED;
}

static state uhci_start(struct usb_host *host)
{

    return -ENOERR;
}

static void uhci_endpoint_disable(struct usb_host *host,
                                  struct usb_endpoint *ep)
{
    struct uhci_host *uhci = host_to_uhci(host);
    irqflags_t irqflags;

    spin_lock_irqsave(&uhci->lock, &irqflags);



    spin_unlock_irqrestore(&uhci->lock, &irqflags);
}

#include "uhci/uhci-pci.c"
