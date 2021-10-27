/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i810-display"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <size.h>
#include <initcall.h>
#include <kmalloc.h>
#include <ioremap.h>
#include <driver/pci.h>
#include <driver/video.h>
#include <driver/video/i810.h>
#include <driver/video/vesa.h>
#include <printk.h>

#include <asm/delay.h>
#include <asm/proc.h>
#include <asm/io.h>

struct i810_device {
    struct video_device video;
    void *mmio, *region;
    int byteseq;
};

#define video_to_idev(vdev) \
    container_of(vdev, struct i810_device, video)

#define I810_TIMEOUT    10000000
#define I810_MMIO_SZ    0x80000

static __always_inline uint8_t
i810_readb(struct i810_device *gdev, uint32_t reg)
{
    return readb(gdev->mmio + reg);
}

static __always_inline uint16_t
i810_readw(struct i810_device *gdev, uint32_t reg)
{
    return readw(gdev->mmio + reg);
}

static __always_inline uint32_t
i810_readl(struct i810_device *gdev, uint32_t reg)
{
    return readl(gdev->mmio + reg);
}

static __always_inline void
i810_writeb(struct i810_device *gdev, uint32_t reg, uint8_t val)
{
    writeb(gdev->mmio + reg, val);
}

static __always_inline void
i810_writew(struct i810_device *gdev, uint32_t reg, uint16_t val)
{
    writew(gdev->mmio + reg, val);
}

static __always_inline void
i810_writel(struct i810_device *gdev, uint32_t reg, uint32_t val)
{
    writel(gdev->mmio + reg, val);
}

/*** Intel 810/815 I2C operation ***/

// static bool i810_i2c_scl_get()
// {

// }
// static void i810_i2c_scl_set()
// {

// }
// static bool i810_i2c_sda_get()
// {

// }
// static void i810_i2c_sda_set()
// {

// }

/**
 * i810_screen_enable - turns off/on display.
 * @idev: i810 device to enable
 * @enable: on or off
 */
static void i810_screen_enable(struct i810_device *idev, bool enable)
{
    unsigned int timeout = I810_TIMEOUT;
    uint8_t val;

    i810_writeb(idev, I810_SR_INDEX, I810_SR01);
    val = i810_readb(idev, I810_SR_DATA);
    val = enable ? val & ~I810_SR01_SCR_OFF : val | I810_SR01_SCR_OFF;

    while ((i810_readb(idev, I810_SR_DATA) & 0xfff) && timeout--)
        cpu_relax();

    i810_writeb(idev, I810_SR_INDEX, I810_SR01);
    i810_writeb(idev, I810_SR_DATA, val);
}

static void i810_dram_enable(struct i810_device *idev, bool enable)
{
    uint8_t val;

    val = i810_readb(idev, I810_DRAMCH);
    val = enable ? val | I810_DRAMCH_DRAM_ON : val & ~I810_DRAMCH_DRAM_ON;
    i810_writeb(idev, I810_DRAMCH, val);
}

static void i810_protect_enable(struct i810_device *idev, bool enable)
{
    uint8_t val;

    i810_writeb(idev, I810_CR_INDEX_CGA, I810_CR11);
    val = i810_readb(idev, I810_CR_DATA_CGA);
    val = enable ? val | VESA_CR11_LOCK_CR0_CR7
                : val & ~VESA_CR11_LOCK_CR0_CR7;
    i810_writeb(idev, I810_CR_INDEX_CGA, I810_CR11);
    i810_writeb(idev, I810_CR_DATA_CGA, val);
}

// static void i810_pll_set(struct i810_device *idev, uint32_t p, uint32_t m, uint32_t n)
// {
//     uint32_t val;


//     val = i810_readl(idev, I810_MSR_READ);
//     i810_writel(idev, I810_MSR_WRITE, );
// }

static state i810_setmode(struct video_device *vdev)
{
    struct i810_device *idev = video_to_idev(vdev);

    i810_screen_enable(idev, false);
    i810_protect_enable(idev, false);
    i810_dram_enable(idev, false);

    /*  */

    i810_dram_enable(idev, true);
    i810_screen_enable(idev, true);
    i810_protect_enable(idev, true);
    return -ENOERR;
}

static struct video_ops i810_ops = {
    .set_mode = i810_setmode,
};

static state i810_hwinit(struct pci_device *pdev)
{
    struct i810_device *idev = pci_get_devdata(pdev);
    resource_size_t start;// phys, size;

    if (pci_resource_type(pdev, 0) != RESOURCE_MMIO64 ||
        pci_resource_type(pdev, 2) != RESOURCE_MMIO64) {
        dev_err(&pdev->dev, "unknow resource type\n");
        return -ENODEV;
    }

    // if ((size = pci_resource_size(pdev, 0)) > I810_MMIO_SZ) {
    //     phys = pci_resource_start(pdev, 0);
    //     start = pci_resource_start(pdev, 1);
    // } else {
    //     phys = pci_resource_start(pdev, 1);
    //     size = pci_resource_size(pdev, 1);
    //     start = pci_resource_start(pdev, 0);
    // } if (!size) {
    //     dev_warn(&pdev->dev, "device not enabled\n");
    //     return -ENODEV;
    // }

        // phys = pci_resource_start(pdev, 2);
        // size = pci_resource_size(pdev, 2);
        start = pci_resource_start(pdev, 0);

    // idev->video.frame_buffer = dev_ioremap(&pdev->dev, phys, size);
    // if (!idev->video.frame_buffer) {
    //     dev_warn(&pdev->dev, "cannot remap framebuffer\n");
    //     return -ENOMEM;
    // }

    idev->mmio = dev_ioremap(&pdev->dev, start, I810_MMIO_SZ);
    if (!idev->mmio) {
        dev_warn(&pdev->dev, "cannot remap mmio\n");
        return -ENOMEM;
    }

    return -ENOERR;
}

static state i810_probe(struct pci_device *pdev, void *pdata)
{
    struct i810_device *idev;
    state ret;

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;
    pci_set_devdata(pdev, idev);

    if ((ret = i810_hwinit(pdev)))
        return ret;

    idev->video.device = &pdev->dev;
    idev->video.ops = &i810_ops;
    return video_register(&idev->video);
}

static const struct pci_device_id i810_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810_IG1),
      .data = "Intel 82810 (i810 GMCH) SVGA controller" },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810_IG3),
      .data = "Intel 82810-DC100 (i810-DC100 GMCH) SVGA controller", },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810E_IG),
      .data = "Intel 82810E (i810E GMCH) SVGA controller" },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82815_100) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82815_NOAGP) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82815_CGC),
      .data = "Intel 82815 (i815 GMCH) SVGA controller" },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2a42),
      .data = "Intel GM45 SVGA controller" },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2e32),
      .data = "Intel G41 SVGA controller" },
    { }, /* NULL */
};

static struct pci_driver i810_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = i810_ids,
    .probe = i810_probe,
};

static state i810_init(void)
{
    return pci_driver_register(&i810_driver);
}
driver_initcall(i810_init);
