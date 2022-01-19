/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "rtl8139"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <driver/pci.h>
#include <driver/net/rtl8139.h>
#include <asm/delay.h>
#include <asm/io.h>

struct rtl8139_info {
    const char *name;
    uint32_t chipid;
    uint8_t lwake:1;
    uint8_t hltclk:1;
};

struct rtl8139_device {
    const struct rtl8139_info *info;
    void *base;
};

enum rtl8139_verion {
    RTL8139     = 0,
    RTL8139_K   = 1,
    RTL8139A    = 2,
    RTL8139A_G  = 3,
    RTL8139B    = 4,
    RTL8130     = 5,
    RTL8139C    = 6,
    RTL8100     = 7,
    RTL8100B    = 8,
    RTL8101     = 9,
};

#define RTL8139_CHIPID(b30, b29, b28, b27, b26, b23, b22) ( \
    (b30 << 30) | (b29<<29) | (b28<<28) | (b27<<27) |       \
    (b26 << 26) | (b23<<23) | (b22<<22)                     \
)

#define RTL8139_CHIPID_MASK RTL8139_CHIPID(1, 1, 1, 1, 1, 1, 1)

static const struct rtl8139_info rtl8139_infos[] = {
    [RTL8139] = {
        .name = "RTL8139",
        .chipid = RTL8139_CHIPID(1, 0, 0, 0, 0, 0, 0),
        .hltclk = true,
    },
    [RTL8139_K] = {
        .name = "RTL8139 rev K",
        .chipid = RTL8139_CHIPID(1, 1, 0, 0, 0, 0, 0),
        .hltclk = true,
    },
    [RTL8139A] = {
        .name = "RTL8139A",
        .chipid = RTL8139_CHIPID(1, 1, 1, 0, 0, 0, 0),
        .hltclk = true,
    },
    [RTL8139A_G] = {
        .name = "RTL8139A rev G",
        .chipid = RTL8139_CHIPID(1, 1, 1, 0, 0, 1, 0),
        .hltclk = true,
    },
    [RTL8139B] = {
        .name = "RTL8139B",
        .chipid = RTL8139_CHIPID(1, 1, 1, 1, 0, 0, 0),
        .lwake = true,
    },
    [RTL8130] = {
        .name = "RTL8130",
        .chipid = RTL8139_CHIPID(1, 1, 1, 1, 1, 0, 0),
        .lwake = true,
    },
    [RTL8139C] = {
        .name = "RTL8139C",
        .chipid = RTL8139_CHIPID(1, 1, 1, 0, 1, 0, 0),
        .lwake = true,
    },
    [RTL8100] = {
        .name = "RTL8100",
        .chipid = RTL8139_CHIPID(1, 1, 1, 1, 0, 1, 0),
        .lwake = true,
    },
    [RTL8100B] = {
        .name = "RTL8100B/8139D",
        .chipid = RTL8139_CHIPID(1, 1, 1, 0, 1, 0, 1),
        .lwake = true, .hltclk = true,
    },
    [RTL8101] = {
        .name = "RTL8101",
        .chipid = RTL8139_CHIPID(1, 1, 1, 0, 1, 1, 1),
        .lwake = true,
    },
};

#define RTL8139_IOMEM_OP(name, type)                \
static __always_inline type                         \
rtl8139_read##name(struct rtl8139_device *rdev,     \
                 unsigned int reg)                  \
{                                                   \
    return ioread##name(rdev->base + reg);          \
}                                                   \
static __always_inline void                         \
rtl8139_write##name(struct rtl8139_device *rdev,    \
                  unsigned int reg, type val)       \
{                                                   \
    iowrite##name(rdev->base + reg, val);           \
}                                                   \
static __always_inline type                         \
rtl8139_sync##name(struct rtl8139_device *rdev,     \
                  unsigned int reg, type val)       \
{                                                   \
    iowrite##name(rdev->base + reg, val);           \
    return ioread##name(rdev->base + reg);          \
}

RTL8139_IOMEM_OP(b, uint8_t);
RTL8139_IOMEM_OP(w, uint16_t);
RTL8139_IOMEM_OP(l, uint32_t);

static bool rtl8139_reset(struct rtl8139_device *rdev)
{
    unsigned int count = 10000;
    uint8_t val;

    rtl8139_writeb(rdev, RTL8139_CHIPCMD, RTL8139_CHIPCMD_RESET);
    while (--count) {
        val = rtl8139_readb(rdev, RTL8139_CHIPCMD);
        if (!(val & RTL8139_CHIPCMD_RESET))
            break;
        udelay(1);
    }

    return !!count;
}

static state rtl8139_hw_init(struct pci_device *pdev)
{
    struct rtl8139_device *rdev = pci_get_devdata(pdev);
    unsigned int count;
    uint32_t val;

    /* bring old chips out of low-power mode. */
    rtl8139_writeb(rdev, RTL8139_HLTCLK, 'R');

    /* check for missing/broken hardware */
    if ((val = rtl8139_readl(rdev, RTL8139_TXCONFIG)) == UINT32_MAX) {
        pci_err(pdev, "chip not responding, ignoring board\n");
        return -EIO;
    }

    /* get rtl8139 card version */
    val &= RTL8139_CHIPID_MASK;
    for (count = 0; count < ARRAY_SIZE(rtl8139_infos); ++count) {
        if (val == rtl8139_infos[count].chipid) {
            rdev->info = &rtl8139_infos[count];
            break;
        }
    } if (!rdev->info) {
        pci_warn(pdev, "unknown chipid version (0x%x), assuming RTL8139\n", val);
        rdev->info = &rtl8139_infos[0];
        count = 0;
    }

    pci_info(pdev, "version %s\n", rdev->info->name);

    /* wake up rtl8139 card */
    if (count >= RTL8139B) {
        uint8_t cmp = val = rtl8139_readb(rdev, RTL8139_CONFIG1);
        if (rdev->info->lwake && (cmp & RTL8139_CONFIG1_LWAKE))
            val &= ~RTL8139_CONFIG1_LWAKE;
        val |= RTL8139_CONFIG1_PM_ENABLE;
        if (val != cmp) {
            rtl8139_writeb(rdev, RTL8139_CFG9346, RTL8139_CFG9346_UNLOCK);
            rtl8139_writeb(rdev, RTL8139_CONFIG1, val);
            rtl8139_writeb(rdev, RTL8139_CFG9346, RTL8139_CFG9346_LOCK);
        }
        if (rdev->info->lwake) {
            val = rtl8139_readb(rdev, RTL8139_CONFIG4);
            if (val & RTL8139_CONFIG4_LWPTN) {
                rtl8139_writeb(rdev, RTL8139_CFG9346, RTL8139_CFG9346_UNLOCK);
                rtl8139_writeb(rdev, RTL8139_CONFIG4, val & ~RTL8139_CONFIG4_LWPTN);
                rtl8139_writeb(rdev, RTL8139_CFG9346, RTL8139_CFG9346_LOCK);
            }
        }
    } else {
        pci_debug(pdev, "old chip wakeup\n");
        val = rtl8139_readb(rdev, RTL8139_CONFIG1);
        val &= ~(RTL8139_CONFIG1_SLEEP | RTL8139_CONFIG1_PWRDN);
        rtl8139_writeb(rdev, RTL8139_CONFIG1, val);
    }

    if (!rtl8139_reset(rdev)) {
        pci_err(pdev, "chip reset timeout\n");
        return -EBUSY;
    }

    return -ENOERR;
}

static state rtl8139_probe(struct pci_device *pdev, const void *pdata)
{
    struct rtl8139_device *rdev;
    state ret;

    rdev = pci_kzalloc(pdev, sizeof(*rdev), GFP_KERNEL);
    if (!rdev)
        return -ENOMEM;

    rdev->base = pci_resource_ioremap(pdev, 0, 0);
    if (!rdev->base)
        return -ENOMEM;

    pci_set_devdata(pdev, rdev);

    // ret = pci_device_enable(pdev);
    // if (ret) {
    //     pci_err(pdev, "enable device fail\n");
    //     return -ENODEV;
    // }

    ret = rtl8139_hw_init(pdev);
    if (ret)
        goto error;

    return -ENOERR;

error:
    return ret;
}

static struct pci_device_id rtl8139_pci_tbl[] = {
    { PCI_DEVICE(0x10ec, 0x8139) },
    { PCI_DEVICE(0x10ec, 0x8138) },
    { PCI_DEVICE(0x1113, 0x1211) },
    { PCI_DEVICE(0x1500, 0x1360) },
    { PCI_DEVICE(0x4033, 0x1360) },
    { PCI_DEVICE(0x1186, 0x1300) },
    { PCI_DEVICE(0x1186, 0x1340) },
    { PCI_DEVICE(0x13d1, 0xab06) },
    { PCI_DEVICE(0x1259, 0xa117) },
    { PCI_DEVICE(0x1259, 0xa11e) },
    { PCI_DEVICE(0x14ea, 0xab06) },
    { PCI_DEVICE(0x14ea, 0xab07) },
    { PCI_DEVICE(0x11db, 0x1234) },
    { PCI_DEVICE(0x1432, 0x9130) },
    { PCI_DEVICE(0x02ac, 0x1012) },
    { PCI_DEVICE(0x018a, 0x0106) },
    { PCI_DEVICE(0x126c, 0x1211) },
    { PCI_DEVICE(0x1743, 0x8139) },
    { PCI_DEVICE(0x021b, 0x8139) },
    { PCI_DEVICE(0x16ec, 0xab06) },
    { } /* NULL */
};

static struct pci_driver rtl8139_driver = {
    .driver = {
        .name = DRIVER_NAME
    },
    .id_table = rtl8139_pci_tbl,
    .probe = rtl8139_probe,
};

static state rtl8139_init(void)
{
    return pci_driver_register(&rtl8139_driver);
}
driver_initcall(rtl8139_init);
