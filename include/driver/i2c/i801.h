/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_I2C_I801_H_
#define _DRIVER_I2C_I801_H_

#include <bits.h>

enum i801_pci_registers {
    I801_SMBHSTCFG      = 0x40,
    I801_TCOBASE        = 0x50,
    I801_TCOCTL         = 0x54,
};

enum i801_registers {
    I801_HST_STS        = 0x00, /* RW: Host Status */
    I801_HST_CNT        = 0x02, /* RW: Host Control */
    I801_HST_CMD        = 0x03, /* RW: Host Command */
    I801_XMIT_SLVA      = 0x04, /* RW: Transmit Slave Address */
    I801_HST_DATA0      = 0x05, /* RW: Host Data 0 */
    I801_HST_DATA1      = 0x06, /* RW: Host Data 1 */
    I801_BLOCK_DATA     = 0x07, /* RW: Host Block Data Byte */
    I801_PEC            = 0x08, /* RW: Packet Error Check */
    I801_AUX_STS        = 0x0c, /* RW: Auxiliary Status */
    I801_AUX_CTL        = 0x0d, /* RW: Auxiliary Control */
};

/********************************************************************************************/
/*      Mnemonic                    value           meaning/usage                           */

#define I801_SMBHSTCFG_SPD_WD       BIT(3)          /* SPD Write Disable */
#define I801_SMBHSTCFG_I2C_EN       BIT(2)          /* I2C Host Enable */
#define I801_SMBHSTCFG_SMI_EN       BIT(1)          /* SMBus interrupts generate an SMI */
#define I801_SMBHSTCFG_HST_EN       BIT(0)          /* SMBus Host Enable */

/**
 * All status bits are set by hardware and cleared by the software writing a one to the
 * particular bit position. Writing a 0 to any bit position has no effect.
 */

#define I801_HST_STS_BYTE_DONE      BIT(7)          /*  */
#define I801_HST_STS_INUSE_STS      BIT(6)          /*  */
#define I801_HST_STS_SMBALERT_STS   BIT(5)          /*  */
#define I801_HST_STS_FAILED         BIT(4)          /*  */
#define I801_HST_STS_BUS_ERR        BIT(3)          /*  */
#define I801_HST_STS_DEV_ERR        BIT(2)          /*  */
#define I801_HST_STS_INTR           BIT(1)          /*  */
#define I801_HST_STS_HOST_BUSY      BIT(0)          /*  */

#define I801_HST_CNT_PEC_EN         BIT(7)          /*  */
#define I801_HST_CNT_START          BIT(6)          /*  */
#define I801_HST_CNT_LAST_BYTE      BIT(5)          /*  */
#define I801_HST_CNT_SMB_CMD        BIT_RANGE(4, 2) /*  */
#define I801_HST_CNT_SMB_BPROC      BIT_SHIFT(2, 7) /*  */
#define I801_HST_CNT_SMB_I2C_READ   BIT_SHIFT(2, 6) /*  */
#define I801_HST_CNT_SMB_BLOCK      BIT_SHIFT(2, 5) /*  */
#define I801_HST_CNT_SMB_PCALL      BIT_SHIFT(2, 4) /*  */
#define I801_HST_CNT_SMB_WDATA      BIT_SHIFT(2, 3) /*  */
#define I801_HST_CNT_SMB_BDATA      BIT_SHIFT(2, 2) /*  */
#define I801_HST_CNT_SMB_BYTE       BIT_SHIFT(2, 1) /*  */
#define I801_HST_CNT_SMB_QUICK      BIT_SHIFT(2, 0) /*  */
#define I801_HST_CNT_KILL           BIT(1)          /*  */
#define I801_HST_CNT_INTREN         BIT(0)          /*  */

#define I801_AUX_STS_STCO           BIT(1)          /* SMBus TCO Mode */
#define I801_AUX_STS_CRCE           BIT(0)          /* CRC Error */

#define I801_AUX_CTL_E32B           BIT(1)          /* Enable 32-Byte Buffer */
#define I801_AUX_CTL_AAC            BIT(0)          /* Automatically Append CRC */

#endif /* _DRIVER_I2C_I801_H_ */
