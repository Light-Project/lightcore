/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MTD_INTEL_H_
#define _DRIVER_MTD_INTEL_H_

#include <bits.h>

enum intel_spi_resisters {
    INTEL_SPI_BF        = 0x00,     /* SPI BIOS MMIO PRI */
    INTEL_SPI_HSCTL     = 0x04,     /* Hardware Sequencing Flash Status and Control */
    INTEL_SPI_ADDR      = 0x08,     /* Flash Address */
    INTEL_SPI_DLOCK     = 0x0c,     /* Discrete Lock Bits */
    INTEL_SPI_DATA      = 0x10,     /* Flash Data */
    INTEL_SPI_RACC      = 0x50,     /* Flash Region Access Permissions */
    INTEL_SPI_FREG      = 0x54,     /* Flash Region */
};

#define INTEL_SPI_DATA(index)   ((index) * 4)
#define INTEL_SPI_SS(index)     ((index) * 4)
#define INTEL_SPI_PR(index)     ((index) * 4)

/********************************************************************************************/
/*      Mnemonic                        value               meaning/usage                   */

#define INTEL_SPI_HSCTL_FDBC            BIT_RANGE(29, 24)   /* Flash Data Byte Count */
#define INTEL_SPI_HSCTL_RDSD            BIT_SHIFT(17, 8)    /* FLASH Cycle: Read status */
#define INTEL_SPI_HSCTL_WRSR            BIT_SHIFT(17, 7)    /* FLASH Cycle: Write status */
#define INTEL_SPI_HSCTL_RDID            BIT_SHIFT(17, 6)    /* FLASH Cycle: Read JEDEC ID */
#define INTEL_SPI_HSCTL_ERASE_64K       BIT_SHIFT(17, 4)    /* FLASH Cycle: 64k Sector erase */
#define INTEL_SPI_HSCTL_ERASE           BIT_SHIFT(17, 3)    /* FLASH Cycle: 4k Block Erase */
#define INTEL_SPI_HSCTL_WRITE           BIT_SHIFT(17, 2)    /* FLASH Cycle: Write (1 up to 64 bytes by setting FDBC) */
#define INTEL_SPI_HSCTL_READ            BIT_SHIFT(17, 0)    /* FLASH Cycle: Read (1 up to 64 bytes by setting FDBC) */
#define INTEL_SPI_HSCTL_FCYCLE          BIT_RANGE(20, 17)   /* FLASH Cycle Mask */
#define INTEL_SPI_HSCTL_FGO             BIT(16)             /* Flash Cycle Go */
#define INTEL_SPI_HSCTL_FLOCKDN         BIT(15)             /* Flash Configuration Lock-Down */
#define INTEL_SPI_HSCTL_FDV             BIT(14)             /* Flash Descriptor Valid */
#define INTEL_SPI_HSCTL_SCIP            BIT(5)              /* SPI Cycle In Progress */
#define INTEL_SPI_HSCTL_AEL             BIT(2)              /* Access Error Log */
#define INTEL_SPI_HSCTL_FCERR           BIT(1)              /* Flash Cycle Error  */
#define INTEL_SPI_HSCTL_FDONE           BIT(0)              /* Flash Cycle Done */

#define INTEL_SPI_PR_WPE                BIT(31)             /* Write Protection Enable */
#define INTEL_SPI_PR_LMT                BIT_RANGE(30, 16)   /* Protected Range Limit */
#define INTEL_SPI_PR_RPE                BIT(15)             /* Read Protection Enable */
#define INTEL_SPI_PR_BASE               BIT_RANGE(14, 0)    /* Protected Range Base */

#define INTEL_SPI_SSCTL_FSMIE           BIT(23)
#define INTEL_SPI_SSCTL_DS              BIT(22)
#define INTEL_SPI_SSCTL_DBC_SHIFT       16
#define INTEL_SPI_SSCTL_SPOP            BIT(11)
#define INTEL_SPI_SSCTL_ACS             BIT(10)
#define INTEL_SPI_SSCTL_SCGO			BIT(9)
#define INTEL_SPI_SSCTL_COP_SHIFT		12
#define INTEL_SPI_SSCTL_FRS             BIT(7)
#define INTEL_SPI_SSCTL_DOFRS           BIT(6)
#define INTEL_SPI_SSCTL_AEL			    BIT(4)
#define INTEL_SPI_SSCTL_FCERR           BIT(3)
#define INTEL_SPI_SSCTL_FDONE           BIT(2)
#define INTEL_SPI_SSCTL_SCIP            BIT(0)

/* Extended parameters */
#define INTEL_SPI_HSCTL_FDOPSS          BIT(13)             /* Flash Descriptor Override Pin-Strap Status */
#define INTEL_SPI_HSCTL_PRR34           BIT(12)             /* PRR3 PRR4 Lock-Down */
#define INTEL_SPI_HSCTL_WRSDIS          BIT(11)             /* Flash Descriptor Valid */

#define INTEL_SPI_BUFFER_SIZE           64
#define INTEL_SPI_BYT_MATCH_ID          "intel,spi-byt"
#define INTEL_SPI_LPT_MATCH_ID          "intel,spi-lpt"
#define INTEL_SPI_BXT_MATCH_ID          "intel,spi-bxt"
#define INTEL_SPI_CNL_MATCH_ID          "intel,spi-cnl"

#endif /* _DRIVER_MTD_INTEL_ */
