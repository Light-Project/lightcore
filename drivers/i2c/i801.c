/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "i801-i2c"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <ioops.h>
#include <delay.h>
#include <ticktime.h>
#include <driver/pci.h>
#include <driver/i2c.h>
#include <driver/i2c/i801.h>
#include <printk.h>

#define CAPABILITY_IRQ          BIT(0)
#define CAPABILITY_BLOCK_BUFF   BIT(1)
#define I801_TIMEOUT            SYSTICK_FREQ / 5

#define STATUS_ERROR (I801_HST_STS_FAILED | I801_HST_STS_BUS_ERR | I801_HST_STS_DEV_ERR)
#define STATUS_FLAGS (STATUS_ERROR | I801_HST_STS_BYTE_DONE | I801_HST_STS_INTR)

struct i801_device {
    struct i2c_host host;
    resource_size_t base;
    unsigned int capability;
    struct mutex lock;
};

#define i2c_to_i801(ptr) \
    container_of(ptr, struct i801_device, host)

#define I801_PMIO_OP(name, type)                                        \
static __always_inline type                                             \
i801_read##name(struct i801_device *idev, unsigned int reg)             \
{                                                                       \
    return in##name##_p(idev->base + reg);                              \
}                                                                       \
                                                                        \
static __always_inline void                                             \
i801_write##name(struct i801_device *idev, unsigned int reg, type val)  \
{                                                                       \
    out##name##_p(idev->base + reg, val);                               \
}

I801_PMIO_OP(b, uint8_t)
I801_PMIO_OP(w, uint16_t)

static state i801_check_ready(struct i801_device *idev)
{
    uint8_t value;

    value = i801_readb(idev, I801_HST_STS);
    if (value & I801_HST_STS_HOST_BUSY) {
        dev_err(idev->host.dev, "transfer failed because SMBus host is busy\n");
        return -EBUSY;
    }

    value &= STATUS_FLAGS;
    if (value) {
        dev_debug(idev->host.dev, "clear host status register value (%#04x)\n", value);
        i801_writeb(idev, I801_HST_STS, value);
    }

    if (idev->host.capability & I2C_FUNC_SMBUS_PEC) {
        value = i801_readb(idev, I801_AUX_STS);
        if (value) {
            dev_debug(idev->host.dev, "clear Auxiliary status register value (%#04x)\n", value);
            i801_writeb(idev, I801_AUX_STS, value);
        }
    }

    return -ENOERR;
}

static state i801_wait_intr(struct i801_device *idev)
{
    unsigned int timeout = I801_TIMEOUT + 1;
    uint8_t val;

    while (--timeout) {
        val = i801_readb(idev, I801_HST_STS);
        if (!(val & I801_HST_STS_HOST_BUSY) &&
            (val &= STATUS_ERROR & I801_HST_STS_INTR))
            break;
        msleep(1);
    }

    return timeout ? val : -ETIMEDOUT;
}

static state i801_status_dump(struct i801_device *idev, state error)
{
    state retval = -ENOERR;
    uint8_t val;

    if (unlikely(error < 0)) {
        dev_err(idev->host.dev, "transaction timeout\n");

        i801_writeb(idev, I801_HST_CNT, I801_HST_CNT_KILL);
        msleep(2);
        i801_writeb(idev, I801_HST_CNT, 0);

        val = i801_readb(idev, I801_HST_STS);
        if ((val & I801_HST_STS_HOST_BUSY) || !(val & I801_HST_STS_HOST_BUSY))
            dev_err(idev->host.dev, "failed terminating the transaction\n");

        return -ETIMEDOUT;
    }

    if (error & I801_HST_STS_HOST_BUSY) {
        dev_err(idev->host.dev, "transaction failed\n");
        retval = -EIO;
    }

    if (error & I801_HST_STS_DEV_ERR) {
        dev_debug(idev->host.dev, "no response\n");
        retval = -ENXIO;
    }

    if (error & I801_HST_STS_BUS_ERR) {
        dev_err(idev->host.dev, "lost arbitration\n");
        retval = -EAGAIN;
    }

    return retval;
}

static state i801_transfer(struct i801_device *idev, uint8_t xact)
{
    state retval;

    retval = i801_check_ready(idev);
    if (retval)
        return retval;

    if (idev->capability & CAPABILITY_IRQ) {
        i801_writeb(idev, I801_HST_CNT, I801_HST_CNT_START | I801_HST_CNT_INTREN | xact);

    }

    i801_writeb(idev, I801_HST_CNT, I801_HST_CNT_START | xact);
    retval = i801_wait_intr(idev);

    return i801_status_dump(idev, retval);
}

static state i801_block_transfer(struct i801_device *idev, bool write,
                                 unsigned int type, union smbus_transfer *trans)
{




    return -ENOERR;
}

static state i801_smbus_transfer(struct i2c_host *host, uint16_t addr, unsigned short flags, unsigned int dir,
                                 uint8_t cmd, unsigned int type, union smbus_transfer *trans)
{
    struct i801_device *idev = i2c_to_i801(host);
    uint8_t xact, block = false;
    state retval = -ENOERR;

    mutex_lock(&idev->lock);

    switch (type) {
        case SMBUS_QUICK:
            i801_writeb(idev, I801_XMIT_SLVA, (addr << 1) | (dir == SMBUS_WRITE));
            xact = I801_HST_CNT_SMB_QUICK;
            break;

        case SMBUS_BYTE:
            i801_writeb(idev, I801_XMIT_SLVA, (addr << 1) | (dir == SMBUS_WRITE));
            if (dir == SMBUS_WRITE)
                i801_writeb(idev, I801_HST_CMD, cmd);
            xact = I801_HST_CNT_SMB_BYTE;
            break;

        case SMBUS_BYTE_DATA:
            i801_writeb(idev, I801_XMIT_SLVA, (addr << 1) | (dir == SMBUS_WRITE));
            i801_writeb(idev, I801_HST_CMD, cmd);
            if (dir == SMBUS_WRITE)
                i801_writeb(idev, I801_HST_DATA0, trans->byte);
            xact = I801_HST_CNT_SMB_BDATA;
            break;

        case SMBUS_WORD_DATA:
            i801_writeb(idev, I801_XMIT_SLVA, (addr << 1) | (dir == SMBUS_WRITE));
            i801_writeb(idev, I801_HST_CMD, cmd);
            if (dir == SMBUS_WRITE) {
                i801_writeb(idev, I801_HST_DATA0, trans->word & 0xff);
                i801_writeb(idev, I801_HST_DATA1, trans->word >> 8);
            }
            xact = I801_HST_CNT_SMB_WDATA;
            break;

        case SMBUS_BLOCK_DATA:
            i801_writeb(idev, I801_XMIT_SLVA, (addr << 1) | (dir == SMBUS_WRITE));
            i801_writeb(idev, I801_HST_CMD, cmd);
            block = true;
            break;

        case SMBUS_PROC_CALL:
            i801_writeb(idev, I801_XMIT_SLVA, addr << 1);
            i801_writeb(idev, I801_HST_CMD, cmd);
            i801_writeb(idev, I801_HST_DATA0, trans->word & 0xff);
            i801_writeb(idev, I801_HST_DATA1, trans->word >> 8);
            xact = I801_HST_CNT_SMB_PCALL;
            dir = SMBUS_READ;
            break;

        case SMBUS_BLOCK_PROC_CALL:
            i801_writeb(idev, I801_XMIT_SLVA, addr << 1);
            i801_writeb(idev, I801_HST_CMD, cmd);
            xact = I801_HST_CNT_SMB_BPROC;
            break;

        case SMBUS_I2C_BLOCK_DATA:
            break;

        default:
            dev_err(host->dev, "unsupported transaction %u\n", type);
            retval = -EINVAL;
            goto exit;
    }

    if (block)
        retval = i801_block_transfer(idev, dir, type, trans);
    else
        retval = i801_transfer(idev, xact);

    if (retval || dir || block || xact == I801_HST_CNT_SMB_QUICK)
        goto exit;

    switch (xact) {
        case I801_HST_CNT_SMB_BYTE: case I801_HST_CNT_SMB_BDATA:
            trans->byte = i801_readb(idev, I801_HST_DATA0);
            break;

        case I801_HST_CNT_SMB_WDATA: case I801_HST_CNT_SMB_PCALL:
            trans->word = i801_readb(idev, I801_HST_DATA0);
            trans->word |= i801_readb(idev, I801_HST_DATA1) << 16;
            break;
    }

exit:
    i801_writeb(idev, I801_HST_STS, STATUS_FLAGS | I801_HST_STS_INUSE_STS);
    mutex_unlock(&idev->lock);
    return retval;
}

static struct i2c_ops i801_ops = {
    .smbus_transfer = i801_smbus_transfer,
};

static void i801_hwinit(struct pci_device *pdev)
{
    uint8_t value;

    value = pci_config_readb(pdev, I801_SMBHSTCFG);
    value |= I801_SMBHSTCFG_HST_EN; /* smbus timing */
    value &= ~I801_SMBHSTCFG_I2C_EN;
    pci_config_writeb(pdev, I801_SMBHSTCFG, value);

    if (value & I801_SMBHSTCFG_SMI_EN)
        pci_info(pdev, "smbus using smi interrupt\n");

    if (value & I801_SMBHSTCFG_SPD_WD)
        pci_debug(pdev, "spd write disable is open\n");
}

static state i801_i2c_probe(struct pci_device *pdev, const void *pdata)
{
    struct i801_device *idev;

    if (pci_resource_type(pdev, 4) != RESOURCE_PMIO)
        return -ENXIO;

    idev = pci_kzalloc(pdev, sizeof(*idev), GFP_KERNEL);
    if (!idev)
        return -ENOMEM;

    idev->base = pci_resource_start(pdev, 4);
    idev->host.dev = &pdev->dev;
    idev->host.ops = &i801_ops;
    pci_set_devdata(pdev, idev);

    i801_hwinit(pdev);
    return i2c_host_register(&idev->host);
}

static struct pci_device_id i801_i2c_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801AA_3) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801AB_3) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801BA_2) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801CA_3) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801DB_3) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82801EB_3) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ESB_4) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH6_16) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7_17) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ESB2_17) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH8_5) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9_6) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_EP80579_1) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH10_4) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH10_5) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_5_3400_SERIES_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_COUGARPOINT_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PATSBURG_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PATSBURG_SMBUS_IDF0) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PATSBURG_SMBUS_IDF1) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PATSBURG_SMBUS_IDF2) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_DH89XXCC_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PANTHERPOINT_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LYNXPOINT_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_AVOTON_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WELLSBURG_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WELLSBURG_SMBUS_MS0) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WELLSBURG_SMBUS_MS1) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WELLSBURG_SMBUS_MS2) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_COLETOCREEK_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_GEMINILAKE_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WILDCATPOINT_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BAYTRAIL_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BRASWELL_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_H_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CDF_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_DNV_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_EBG_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BROXTON_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LEWISBURG_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LEWISBURG_SSKU_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_KABYLAKE_PCH_H_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CANNONLAKE_H_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CANNONLAKE_LP_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICELAKE_LP_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICELAKE_N_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_COMETLAKE_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_COMETLAKE_H_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_COMETLAKE_V_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ELKHART_LAKE_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_TIGERLAKE_LP_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_TIGERLAKE_H_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_JASPER_LAKE_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ALDER_LAKE_S_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ALDER_LAKE_P_SMBUS) },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ALDER_LAKE_M_SMBUS) },
    { }, /* NULL */
};

static struct pci_driver i801_i2c_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = i801_i2c_ids,
    .probe = i801_i2c_probe,
};

static state i801_i2c_init(void)
{
    return pci_driver_register(&i801_i2c_driver);
}
driver_initcall(i801_i2c_init);
