/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_ATA_ATAREG_H_
#define _DRIVER_ATA_ATAREG_H_

enum ata_registers {
    /* Command Block Regs */
    ATA_REG_DATA        = 0x00, /* (RW): PIO data bytes */
    ATA_REG_ERR         = 0x01, /* (RO): used to retrieve any error generated by the last ATA command executed */
    ATA_REG_FEATURE     = 0x01, /* (WO): used to control command specific interface features */
    ATA_REG_NSECT       = 0x02, /* (RW): number of sectors to read/write (0 is a special value) */
    ATA_REG_LBAL        = 0x03, /* (RW): Partial Disk Sector address */
    ATA_REG_LBAM        = 0x04, /* (RW): Partial Disk Sector address */
    ATA_REG_LBAH        = 0x05, /* (RW): Partial Disk Sector address */
    ATA_REG_DEVSEL      = 0x06, /* (RW): used to select a drive and/or head. Supports extra address/flag bits */
    ATA_REG_STATUS      = 0x07, /* (RO): used to read the current status */
    ATA_REG_CMD         = 0x07, /* (WO): used to send ATA commands to the device */

    /* Control Block Regs */
    ATA_REG_ASTAT       = 0x00, /* (RO): alternate status */
    ATA_REG_DEVCTL      = 0x00, /* (WO): device control */
    ATA_REG_DEVADR      = 0x01, /* (RO): device address */
};

/****************************************************************************************/
/*      Mnemonic                value             meaning/usage                         */

#define ATA_DEVSEL_IBM          BIT(7) | BIT(5) /* 512 byte sectors & ECC  */
#define ATA_DEVSEL_LBA          BIT(6)          /* Uses CHS addressing if clear or LBA addressing if set */
#define ATA_DEVSEL_DEV          BIT(4)          /* Selects the drive number */

#define ATA_STATUS_BSY          BIT(7)          /* Busy */
#define ATA_STATUS_DRDY         BIT(6)          /* Drive ready */
#define ATA_STATUS_DF           BIT(5)          /* Drive write fault */
#define ATA_STATUS_DSC          BIT(4)          /* Drive seek complete */
#define ATA_STATUS_DRQ          BIT(3)          /* Data request ready */
#define ATA_STATUS_CORR         BIT(2)          /* Corrected data */
#define ATA_STATUS_IDX          BIT(1)          /* Index */
#define ATA_STATUS_ERR          BIT(0)          /* Error */

#define ATA_ERR_ICRC            BIT(7)          /* ATA ultra DMA bad CRC */
#define ATA_ERR_BBK             BIT(7)          /* ATA bad block */
#define ATA_ERR_UNC             BIT(6)          /* ATA uncorrected error */
#define ATA_ERR_MC              BIT(5)          /* ATA media change */
#define ATA_ERR_IDNF            BIT(4)          /* ATA id not found */
#define ATA_ERR_MCR             BIT(3)          /* ATA media change request */
#define ATA_ERR_ABRT            BIT(2)          /* ATA command aborted */
#define ATA_ERR_TKZNF           BIT(1)          /* ATA track 0 not found */
#define ATA_ERR_AMNF            BIT(0)          /* ATA address mark not found */

#define ATA_DEVCTL_HD15         BIT(3)          /* Bit should always be set to one */
#define ATA_DEVCTL_SRST         BIT(2)          /* Soft reset */
#define ATA_DEVCTL_NIEN         BIT(1)          /* Disable interrupts */

#endif /* _DRIVER_ATA_ATAREG_H_ */
