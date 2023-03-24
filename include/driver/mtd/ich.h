/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MTD_ICH_H_
#define _DRIVER_MTD_ICH_H_

enum ich_spi_registers {
    ICH_SPI_BFPR        = 0x00,     /* BIOS Flash Primary Region */
    ICH_SPI_HSFS        = 0x04,     /* Hardware Sequencing Flash Status */
    ICH_SPI_HSFC        = 0x06,     /* Hardware Sequencing Flash Control */
    ICH_SPI_FADDR       = 0x08,     /* Flash Address */
    ICH_SPI_FDATA       = 0x10,     /* Flash Data */
    ICH_SPI_FRACC       = 0x50,     /* Flash Region Access Permissions */
    ICH_SPI_FREG        = 0x54,     /* Flash Region */
    ICH_SPI_FPR         = 0x74,     /* Flash Protected Range */
    ICH_SPI_SSFS        = 0x90,     /* Software Sequencing Flash Status */
    ICH_SPI_SSFC        = 0x91,     /* Software Sequencing Flash Control */
    ICH_SPI_PREOP       = 0x94,     /* Prefix Opcode Configuration */
    ICH_SPI_OPTYPE      = 0x96,     /* Opcode Type Configuration */
    ICH_SPI_OPMENU      = 0x98,     /* Opcode Menu Configuration */
    ICH_SPI_FDOC        = 0xb0,     /* Flash Descriptor Observability Control */
    ICH_SPI_FDOD        = 0xb4,     /* Flash Descriptor Observability Data */
};

#define ICH_SPI_FDATA_NR        16
#define ICH_SPI_FDATA(index)    (ICH_SPI_FDATA + ((index) * 4))

#define ICH_SPI_FREG_NR         5
#define ICH_SPI_FREG(index)     (ICH_SPI_FREG + ((index) * 4))

#define ICH_SPI_FPR_NR          5
#define ICH_SPI_FPR(index)      (ICH_SPI_FPR + ((index) * 4))

/********************************************************************************************/
/*      Mnemonic                        value               meaning/usage                   */

#define ICH_SPI_BFPR_PRL                BIT_RANGE(28, 16)   /* BIOS Flash Primary Region Limit */
#define ICH_SPI_BFPR_PRB                BIT_RANGE(12, 0)    /* BIOS Flash Primary Region Base */

#define ICH_SPI_HSFS_FLOCKDN            BIT(15)             /* Flash Configuration Lock-Down */
#define ICH_SPI_HSFS_FDV                BIT(14)             /* Flash Descriptor Valid */
#define ICH_SPI_HSFS_FDOPSS             BIT(13)             /* Flash Descriptor Override Pin-Strap Status */
#define ICH_SPI_HSFS_SCIP               BIT(5)              /* SPI Cycle In Progress */
#define ICH_SPI_HSFS_BERASE             BIT_RANGE(4, 3)     /* Block/Sector Erase Size */
#define ICH_SPI_HSFS_AEL                BIT(2)              /* Access Error Log */
#define ICH_SPI_HSFS_FCERR              BIT(1)              /* Flash Cycle Error */
#define ICH_SPI_HSFS_DONE               BIT(0)              /* Flash Cycle Done */

#define ICH_SPI_HSFC_FSMIE              BIT(15)             /* Flash SPI SMI# Enable */
#define ICH_SPI_HSFC_FDBC               BIT_RANGE(13, 8)    /* Flash Data Byte Count */
#define ICH_SPI_HSFC_FCYCLE             BIT_RANGE(2, 1)     /* FLASH Cycle Mask */
#define ICH_SPI_HSFC_FCYCLE_ERASE       BIT_SHIFT(1, 3)     /* FLASH Cycle: Block Erase */
#define ICH_SPI_HSFC_FCYCLE_WRITE       BIT_SHIFT(1, 2)     /* FLASH Cycle: Write (len by setting FDBC) */
#define ICH_SPI_HSFC_FCYCLE_READ        BIT_SHIFT(1, 0)     /* FLASH Cycle: Read (len by setting FDBC) */
#define ICH_SPI_HSFC_FGO                BIT(0)              /* Flash Cycle Go */

#define ICH_SPI_FPR_WPE                 BIT(31)             /* Write Protection Enable */
#define ICH_SPI_FPR_LMT                 BIT_RANGE(30, 16)   /* Protected Range Limit */
#define ICH_SPI_FPR_RPE                 BIT(15)             /* Read Protection Enable */
#define ICH_SPI_FPR_BASE                BIT_RANGE(12, 0)    /* Protected Range Base */

#define ICH_SPI_SSFS_AEL                BIT(4)              /* Access Error Log */
#define ICH_SPI_SSFS_FCERR              BIT(3)              /* Flash Cycle Error */
#define ICH_SPI_SSFS_CDS                BIT(2)              /* Cycle Done Status */
#define ICH_SPI_SSFS_SCIP               BIT(0)              /* SPI Cycle In Progress */

#define ICH_SPI_SSFC_SCF                BIT_RANGE(18, 16)   /* SPI Cycle Frequency */
#define ICH_SPI_SSFC_SME                BIT(15)             /* SPI SMI# Enable */
#define ICH_SPI_SSFC_DS                 BIT(14)             /* Data Cycle */
#define ICH_SPI_SSFC_DBC                BIT(13, 8)          /* Data Byte Count */
#define ICH_SPI_SSFC_COP                BIT_RANGE(6, 4)     /* Cycle Opcode Pointer */
#define ICH_SPI_SSFC_SPOP               BIT(3)              /* Sequence Prefix Opcode Pointer */
#define ICH_SPI_SSFC_ACS                BIT(2)              /* Atomic Cycle Sequence */
#define ICH_SPI_SSFC_SCGO               BIT(1)              /* SPI Cycle Go */

#define ICH_SPI_FDOC_FDSS               BIT_RANGE(14, 12)   /* Flash Descriptor Section Select */
#define ICH_SPI_FDOC_FDSI               BIT_RANGE(11, 2)    /* Flash Descriptor Section Index */

#define ICH_SPI_BUFFER_SIZE             64
#define INTEL_SPI_LEG_MATCH_ID          "intel,spi-leg"
#define INTEL_SPI_ICH_MATCH_ID          "intel,spi-ich"

#endif /* _DRIVER_MTD_ICH_H_ */
