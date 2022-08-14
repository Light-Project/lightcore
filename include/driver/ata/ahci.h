/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_ATA_AHCI_H_
#define _DRIVER_ATA_AHCI_H_

#include <types.h>
#include <bits.h>

enum ahci_host_registers {
    AHCI_HCAP               = 0x00,     /* RO: Host Capabilities */
    AHCI_HCTL               = 0x04,     /* RW: Global Host Control */
    AHCI_HIRQ_STAT          = 0x08,     /* RC: Interrupt Status */
    AHCI_HPORTS_IMPL        = 0x0c,     /* RO: Bitmap Of Implemented Ports */
    AHCI_HVERSION           = 0x10,     /* RO: AHCI Spec Version Compliancy */
    AHCI_HEM_LOC            = 0x1c,     /* RW: Enclosure Management location */
    AHCI_HEM_CTL            = 0x20,     /* RW: Enclosure Management Control */
    AHCI_HCAP2              = 0x24,     /* RO: host extended capabilities */
    AHCI_HPORTS_BASE        = 0x100,    /* RW: AHCI Ports Register Base */
};

enum ahci_port_registers {
    AHCI_LST_ADDR           = 0x00,     /* RW: Command List DMA Addr */
    AHCI_LST_ADDR_HI        = 0x04,     /* RW: Command List DMA Addr HI */
    AHCI_FIS_ADDR           = 0x08,     /* RW: Fis RX Buf Addr */
    AHCI_FIS_ADDR_HI        = 0x0c,     /* RW: Fis RX Buf Addr HI */
    AHCI_IRQ_STAT           = 0x10,     /* RC: Interrupt Status */
    AHCI_IRQ_MASK           = 0x14,     /* RW: Interrupt Enable/Disable Mask */
    AHCI_CMD                = 0x18,     /* RW: Port Command */
    AHCI_TFDATA             = 0x20,     /* RO: Taskfile Data */
    AHCI_SIG                = 0x24,     /* RO: Device TF Signature */
    AHCI_SCR_STAT           = 0x28,     /* RO: SATA Phy Register: Sstatus */
    AHCI_SCR_CTL            = 0x2c,     /* RW: SATA Phy Register: Scontrol */
    AHCI_SCR_ERR            = 0x30,     /* RW: SATA Phy Register: Serror */
    AHCI_SCR_ACT            = 0x34,     /* SATA Phy Register: Sactive */
    AHCI_CMD_ISSUE          = 0x38,     /* Command Issue */
    AHCI_SCR_NTF            = 0x3c,     /* SATA PHY Register: SNotification */
    AHCI_FBS                = 0x40,     /* FIS-based Switching */
    AHCI_DEVSLP             = 0x44,     /* Device Sleep */
    AHCI_PORT_SIZE          = 0x80,     /* AHCI Port Register Size */
};

struct ahci_cfis {
    uint8_t reg;
    uint8_t pmp_type;
    uint8_t command;
    uint8_t feature;

    uint8_t lba_low;
    uint8_t lba_mid;
    uint8_t lba_high;
    uint8_t device;

    uint8_t lba_low2;
    uint8_t lba_mid2;
    uint8_t lba_high2;
    uint8_t feature2;

    uint8_t sector_count;
    uint8_t sector_count2;
    uint8_t res1;
    uint8_t control;

    uint8_t res2[64 - 16];
} __packed;

struct ahci_prdt {
    uint32_t base;
    uint32_t baseu;
    uint32_t res;
    uint32_t flags;
} __packed;

struct ahci_cmd_table {
    struct ahci_cfis cfis;      /* 0x00:  */
    uint8_t atapi[0x20];        /* 0x40:  */
    uint8_t res[0x20];          /* 0x60:  */
    struct ahci_prdt prdt[];    /* 0x80:  */
} __packed;

struct ahci_cmd_list {
    uint32_t flags;             /* 0x00:  */
    uint32_t bytes;             /* 0x04: PRD Byte Count */
    uint32_t basel;             /* 0x08: Command Table Base Addr Low */
    uint32_t baseh;             /* 0x0c: Command Table Base Addr High */
    uint32_t res[4];            /* 0x10:  */
} __packed;

struct ahci_fis_table {
    uint8_t dsfis[0x1c];        /* DMA Setup FIS */
    uint8_t res1[0x04];
    uint8_t psfis[0x14];        /* PIO Setup FIS */
    uint8_t res2[0x0c];
    uint8_t rfis[0x14];         /* D2H Register */
    uint8_t res3[0x04];
    uint8_t sdbfis[0x08];       /* Set Device Bits FIS */
    uint8_t ufis[0x40];
    uint8_t res4[0x60];
} __packed;

#define AHCI_CMD_LIST_LEN       32
#define AHCI_CMD_LIST_SIZE      (AHCI_CMD_LIST_LEN * sizeof(struct ahci_cmd_list))

/****************************************************************************************/
/*      Mnemonic                value                 meaning/usage                     */

#define AHCI_HCTL_AHCI_EN       BIT(31)             /* AHCI enabled */
#define AHCI_HCTL_MRSM          BIT(2)              /* MSI Revert to Single Message */
#define AHCI_HCTL_IRQ_EN        BIT(1)              /* global IRQ enable */
#define AHCI_HCTL_RESET         BIT(0)              /* reset controller; self-clear */

#define AHCI_HCAP_64            BIT(31)             /* PCI DAC (64-bit DMA) support */
#define AHCI_HCAP_NCQ           BIT(30)             /* Native Command Queueing */
#define AHCI_HCAP_SNTF          BIT(29)             /* SNotification register */
#define AHCI_HCAP_MPS           BIT(28)             /* Mechanical presence switch */
#define AHCI_HCAP_SSS           BIT(27)             /* Staggered Spin-up */
#define AHCI_HCAP_ALPM          BIT(26)             /* Aggressive Link PM support */
#define AHCI_HCAP_LED           BIT(25)             /* Supports activity LED */
#define AHCI_HCAP_CLO           BIT(24)             /* Command List Override support */
#define AHCI_HCAP_ONLY          BIT(18)             /* Supports AHCI mode only */
#define AHCI_HCAP_PMP           BIT(17)             /* Port Multiplier support */
#define AHCI_HCAP_FBS           BIT(16)             /* FIS-based switching support */
#define AHCI_HCAP_PIO_MULTI     BIT(15)             /* PIO multiple DRQ support */
#define AHCI_HCAP_SSC           BIT(14)             /* Slumber state capable */
#define AHCI_HCAP_PART          BIT(13)             /* Partial state capable */
#define AHCI_HCAP_CCC           BIT(7)              /* Command Completion Coalescing */
#define AHCI_HCAP_EMS           BIT(6)              /* Enclosure Management support */
#define AHCI_HCAP_SXS           BIT(5)              /* Supports External SATA */

#define AHCI_HCAP2_DESO         BIT(5)              /* DevSlp from slumber only */
#define AHCI_HCAP2_SADM         BIT(4)              /* Support aggressive DevSlp */
#define AHCI_HCAP2_SDS          BIT(3)              /* Support device sleep */
#define AHCI_HCAP2_APST         BIT(2)              /* Automatic partial to slumber */
#define AHCI_HCAP2_NVMHCI       BIT(1)              /* NVMHCI supported */
#define AHCI_HCAP2_BOH          BIT(0)              /* BIOS/OS handoff supported */

#define AHCI_IRQ_COLD_PRES      BIT(31)             /* Cold Presence Detect */
#define AHCI_IRQ_TF_ERR         BIT(30)             /* Task File Error */
#define AHCI_IRQ_HBUS_ERR       BIT(29)             /* Host Bus Fatal Error */
#define AHCI_IRQ_HBUS_DATA_ERR  BIT(28)             /* Host Bus Data Error */
#define AHCI_IRQ_IF_ERR         BIT(27)             /* Interface Fatal Error */
#define AHCI_IRQ_IF_NONFATAL    BIT(26)             /* Interface Non-Fatal Error */
#define AHCI_IRQ_OVERFLOW       BIT(24)             /* Xfer Exhausted Available S/G */
#define AHCI_IRQ_BAD_PMP        BIT(23)             /* Incorrect Port Multiplier */
#define AHCI_IRQ_PHYRDY         BIT(22)             /* Phyrdy Changed */
#define AHCI_IRQ_DEV_ILCK       BIT(7)              /* Device Interlock */
#define AHCI_IRQ_CONNECT        BIT(6)              /* Port Connect Change Status */
#define AHCI_IRQ_SG_DONE        BIT(5)              /* Descriptor Processed */
#define AHCI_IRQ_UNK_FIS        BIT(4)              /* Unknown FIS RX'd */
#define AHCI_IRQ_SDB_FIS        BIT(3)              /* Set Device Bits FIS RX'd */
#define AHCI_IRQ_DMAS_FIS       BIT(2)              /* DMA Setup FIS RX'd */
#define AHCI_IRQ_PIOS_FIS       BIT(1)              /* Pio Setup FIS RX'd */
#define AHCI_IRQ_D2H_REG_FIS    BIT(0)              /* D2h Register FIS RX'd */

#define AHCI_CMD_ICC_MASK       BIT_SHIFT(31, 28)   /* I/F ICC State Mask */
#define AHCI_CMD_ICC_SLUMBER    BIT_SHIFT(28, 6)    /* Put I/F In Slumber State */
#define AHCI_CMD_ICC_PARTIAL    BIT_SHIFT(28, 2)    /* Put I/F In Partial State */
#define AHCI_CMD_ICC_ACTIVE     BIT_SHIFT(28, 1)    /* Put I/F In Active State */
#define AHCI_CMD_ATAPI          BIT(24)             /* Device Is Atapi */
#define AHCI_CMD_LIST_ON        BIT(15)             /* Cmd List DMA Engine Running */
#define AHCI_CMD_FIS_ON         BIT(14)             /* FIS DMA Engine Running */
#define AHCI_CMD_FIS_RX         BIT(4)              /* Enable FIS Receive DMA Engine */
#define AHCI_CMD_CLO            BIT(3)              /* Command List Override */
#define AHCI_CMD_POWER_ON       BIT(2)              /* Power Up Device */
#define AHCI_CMD_SPIN_UP        BIT(1)              /* Spin Up Device */
#define AHCI_CMD_START          BIT(0)              /* Enable Port DMA Engine */

#define AHCI_FBS_SDE            BIT(2)              /* FBS single device error */
#define AHCI_FBS_DEC            BIT(1)              /* FBS device error clear */
#define AHCI_FBS_EN             BIT(0)              /* Enable FBS */

#define AHCI_SCR_STAT_IPM       BIT_RANGE(11, 8)    /*  */
#define AHCI_SCR_STAT_SPD       BIT_RANGE(7, 4)     /* Indicates the negotiated interface communication speed. */
#define AHCI_SCR_STAT_SPD_GEN3  BIT_SHIFT(4, 3)     /* Gen 3 communication rate negotiated */
#define AHCI_SCR_STAT_SPD_GEN2  BIT_SHIFT(4, 2)     /* Gen 2 communication rate negotiated */
#define AHCI_SCR_STAT_SPD_GEN1  BIT_SHIFT(4, 1)     /* Gen 1 communication rate negotiated */
#define AHCI_SCR_STAT_SPD_OFF   BIT_SHIFT(4, 0)     /* No device detected and Phy communication not established */
#define AHCI_SCR_STAT_DET_MASK  BIT_RANGE(3, 0)     /* Indicates the interface device detection and Phy state */
#define AHCI_SCR_STAT_DET_OFF   BIT_SHIFT(0, 4)     /* Phy in offline mode as a result of the interface being disabled or running in a BIST loopback mode */
#define AHCI_SCR_STAT_DET_DPDC  BIT_SHIFT(0, 3)     /* Device presence detected and Phy communication established */
#define AHCI_SCR_STAT_DET_DPNC  BIT_SHIFT(0, 1)     /* Device presence detected but Phy communication not established */
#define AHCI_SCR_STAT_DET_NPNC  BIT_SHIFT(0, 0)     /* No device detected and Phy communication not established */

#define AHCI_CMDTF_PRDTL        BIT_RANGE(31, 13)
#define AHCI_CMDTF_PMP          BIT_RANGE(15, 12)
#define AHCI_CMDTF_CLR_BUSY     BIT(10)
#define AHCI_CMDTF_RESET        BIT(8)
#define AHCI_CMDTF_PREFETCH     BIT(7)
#define AHCI_CMDTF_WRITE        BIT(6)
#define AHCI_CMDTF_ATAPI        BIT(5)
#define AHCI_CMDTF_CFL          BIT_RANGE(4, 0)

#endif  /* _DRIVER_ATA_AHCI_H_ */
