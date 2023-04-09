/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "nvme-simple"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <printk.h>
#include <driver/block.h>
#include <driver/pci.h>
#include <driver/ata.h>
#include <driver/nvme.h>
#include <delay.h>
#include <asm/proc.h>
#include <asm/io.h>

struct nvmesim_host {
    struct pci_device *pdev;
    void *base;
};

struct nvmesim_namespace {
    struct block_device blk;
    struct nvmesim_host *host;
    uint32_t ns_index;
};

#define block_to_nvmesim(bdev) \
    container_of(bdev, struct nvmesim_namespace, blk)

static __always_inline uint32_t
nvmesim_readl(struct nvmesim_host *host, unsigned int reg)
{
    return readl(host->base + reg);
}

static __always_inline void
nvmesim_writel(struct nvmesim_host *host, unsigned int reg, uint32_t val)
{
    writel(host->base + reg, val);
}

static __always_inline uint64_t
nvmesim_readq(struct nvmesim_host *host, unsigned int reg)
{
    return readq(host->base + reg);
}

static __always_inline void
nvmesim_writeq(struct nvmesim_host *host, unsigned int reg, uint64_t val)
{
    writeq(host->base + reg, val);
}

static bool nvme_wait_csts(struct nvmesim_host *host, bool ready)
{
    unsigned int timeout = 10;
    uint32_t val;

    while (--timeout) {
        val = nvmesim_readl(host, NVME_CSTS);
        if ((val & NVME_CSTS_RDY) == ready)
            break;
        else if (val & NVME_CSTS_FATAL)
            return false;
    }

    return !!timeout;
}

static state nvmesim_sq_init(struct nvmesim_host *host)
{

}

static state nvmesim_cq_init(struct nvmesim_host *host)
{

}

static state nvmesim_enqueue(struct block_device *bdev, struct block_request *breq)
{
    struct nvmesim_namespace *nns = block_to_nvmesim(bdev);



    return -ENOERR;
}


static struct block_ops atasim_ops = {
    .enqueue = nvmesim_enqueue,
};

static state nvmesim_host_setup(struct pci_device *pdev)
{
    struct nvmesim_host *host = pci_get_devdata(pdev);
    uint64_t cap;
    uint32_t val;

    val = nvmesim_readl(host, NVME_VS);
    pci_info(pdev, "nvme controller version %u.%u.%u\n",
             val >> 16, (val >> 8) & 0xff, val & 0xff);

    cap = nvmesim_readq(host, NVME_CAP);
    if (!(cap & NVME_CAP_CSS_NVME)) {
        pci_info(pdev, "controller not support nvme command set\n");
        return -ENODEV;
    }

    pci_master_enable(pdev);

    /* shutdown nvme controller */
    nvmesim_writel(host, NVME_CC, 0);
    if (!nvme_wait_csts(host, 0)) {
        pci_err(pdev, "fatal error during controller shutdown\n");
        return -EIO;
    }

    return -ENOERR;
}

static state nvmesim_probe(struct pci_device *pdev, const void *pdata)
{
    struct nvmesim_host *host;
    state ret;

    host = pci_kzalloc(pdev, sizeof(*host), GFP_KERNEL);
    if (!host)
        return -ENOMEM;

    host->base = pci_resource_ioremap(pdev, 0, 0);
    if (!host->base)
        return -ENOMEM;

    host->pdev = pdev;
    pci_set_devdata(pdev, host);

    ret = nvmesim_host_setup(pdev);
    if (ret)
        return ret;

    return -ENOERR;
}

static const struct pci_device_id nvmesim_ids[] = {
    { PCI_DEVICE_CLASS(PCI_CLASS_STORAGE_EXPRESS, ~0) },
    { }, /* NULL */
};

static struct pci_driver nvmesim_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = nvmesim_ids,
    .probe = nvmesim_probe,
};

/* priority to use other drivers (sync init) */
static state nvmesim_init(void)
{
    return pci_driver_register(&nvmesim_driver);
}
driver_initcall_sync(nvmesim_init);
