/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MMC_BCM2835_H_
#define _DRIVER_MMC_BCM2835_H_

#include <bits.h>

enum bcm2835_mmc_registers {
    BCM2835_MMC_CMD     = 0x00, /* (RW) Command to SD card */
    BCM2835_MMC_ARG     = 0x04, /* (RW) Argument to SD card */
    BCM2835_MMC_TOUT    = 0x08, /* (RW) Start value for timeout counter */
    BCM2835_MMC_CDIV    = 0x0c, /* (RW) Start value for clock divider */
    BCM2835_MMC_RSP0    = 0x10, /* (RO) SD card response (31:0) */
    BCM2835_MMC_RSP1    = 0x14, /* (RO) SD card response (63:32) */
    BCM2835_MMC_RSP2    = 0x18, /* (RO) SD card response (95:64) */
    BCM2835_MMC_RSP3    = 0x1c, /* (RO) SD card response (127:96) */
    BCM2835_MMC_HSTS    = 0x20, /* (RW) SD host status */
    BCM2835_MMC_VDD     = 0x30, /* (RW) SD card power control */
    BCM2835_MMC_EDM     = 0x34, /* (RW) Emergency Debug Mode */
    BCM2835_MMC_HCFG    = 0x38, /* (RW) Host configuration */
    BCM2835_MMC_HBCT    = 0x3c, /* (RW) Host byte count (debug) */
    BCM2835_MMC_DATA    = 0x40, /* (RW) Data to/from SD card */
    BCM2835_MMC_HBLC    = 0x50, /* (RW) Host block count (SDIO/SDHC) */
};

/********************************************************************************************/
/*      Mnemonic                        value        meaning/usage                          */

#define BCM2835_MMC_CMD_NEW             BIT(15)
#define BCM2835_MMC_CMD_FAIL            BIT(14)
#define BCM2835_MMC_CMD_BUSY_WAIT       BIT(11)
#define BCM2835_MMC_CMD_NO_RESP         BIT(10)
#define BCM2835_MMC_CMD_LONG_RESP       BIT(9)
#define BCM2835_MMC_CMD_WRITE_CMD       BIT(7)
#define BCM2835_MMC_CMD_READ_CMD        BIT(6)
#define BCM2835_MMC_CMD_CMD_MASK        BIT_RANGE(5, 0)

#define BCM2835_MMC_CDIV_MASK           0x7ff

#define BCM2835_MMC_EDM_MASK            0xf
#define BCM2835_MMC_EDM_STARTPOWDOWN    0xf
#define BCM2835_MMC_EDM_WRITEWAIT2      0xd
#define BCM2835_MMC_EDM_GENPULSES       0xc
#define BCM2835_MMC_EDM_WRITESTART2     0xb
#define BCM2835_MMC_EDM_WRITESTART1     0xa
#define BCM2835_MMC_EDM_POWERUP         0x9
#define BCM2835_MMC_EDM_POWERDOWN       0x8
#define BCM2835_MMC_EDM_WRITEWAIT1      0x7
#define BCM2835_MMC_EDM_WRITECRC        0x6
#define BCM2835_MMC_EDM_READCRC         0x5
#define BCM2835_MMC_EDM_READWAIT        0x4
#define BCM2835_MMC_EDM_WRITEDATA       0x3
#define BCM2835_MMC_EDM_READDATA        0x2
#define BCM2835_MMC_EDM_DATAMODE        0x1
#define BCM2835_MMC_EDM_IDENTMODE       0x0

#define BCM2835_MMC_VDD_POWER_ON        0x1

#endif /* _DRIVER_MMC_BCM2835_H_ */
