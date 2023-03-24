/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MMC_SUNXI_H_
#define _DRIVER_MMC_SUNXI_H_

#include <bits.h>

enum sunxi_mmc_registers {
    SUNXI_MMC_GCTL      = 0x0,      // SD Global Control Register
    SUNXI_MMC_CKCR      = 0x4,      // SD Clock Control Register
    SUNXI_MMC_TMOR      = 0x8,      // SD Timeout Register
    SUNXI_MMC_BWDR      = 0xc,      // SD Bus Width Register
    SUNXI_MMC_BKSR      = 0x10,     // SD Block Size Register
    SUNXI_MMC_BYCR      = 0x14,     // SD Byte Count Register
    SUNXI_MMC_CMDR      = 0x18,     // SD Command Register
    SUNXI_MMC_CAGR      = 0x1c,     // SD Command Argument Register
    SUNXI_MMC_RESP0     = 0x20,     // SD Response Register 0
    SUNXI_MMC_RESP1     = 0x24,     // SD Response Register 1
    SUNXI_MMC_RESP2     = 0x28,     // SD Response Register 2
    SUNXI_MMC_RESP3     = 0x2c,     // SD Response Register 3
    SUNXI_MMC_IMKR      = 0x30,     // SD Interrupt Mask Register
    SUNXI_MMC_MISR      = 0x34,     // SD Masked Interrupt Status Register
    SUNXI_MMC_RISR      = 0x38,     // SD Raw Interrupt Status Register
    SUNXI_MMC_STAR      = 0x3c,     // SD Status Register
    SUNXI_MMC_FWLR      = 0x40,     // SD FIFO Water Level Register
    SUNXI_MMC_FUNS      = 0x44,     // SD Function Select Register
    SUNXI_MMC_CBCR      = 0x48,     // SD Transferred CIU Card Byte Count Register
    SUNXI_MMC_BBCR      = 0x4c,     // SD Transferred Host to BIU-FIFO Byte Count Register
    SUNXI_MMC_DBGC      = 0x50,     // SD Debug Control Register
    SUNXI_MMC_A12A      = 0x58,     // SD Auto Command 12 Argument Register
    SUNXI_MMC_HWRST     = 0x78,     // SD Hardware Reset Register
    SUNXI_MMC_DMAC      = 0x80,     // SD DMAC Control Register
    SUNXI_MMC_DLBA      = 0x84,     // SD Descriptor List Base Address Register
    SUNXI_MMC_IDST      = 0x88,     // SD DMAC Status Register
    SUNXI_MMC_IDIE      = 0x8c,     // SD DMAC Interrupt Enable Register
    SUNXI_MMC_CHDA      = 0x90,     // SD Current Host Descriptor Address Register
    SUNXI_MMC_CBDA      = 0x94,     // SD Current Buffer Descriptor Address Register

    SUNXI_MMC_THLDC     = 0x100,    // Card Threshold Control Register
    SUNXI_MMC_DSBD      = 0x10c,    // EMMC4.5 DDR Start Bit Detection Control Register

    SUNXI_MMC_FIFO      = 0x200,    // SD FIFO Register
};

enum SUNXI_mmc_states {
    SUNXI_MMC_IDLE      = 0,
    SUNXI_MMC_WAIT      = 15,
};

/********************************************************************************************/
/*      Mnemonic                        value        meaning/usage                          */

#define SUNXI_MMC_GCTL_FIFO_AC              BIT(31)     // FIFO Access Mode (0:DMA BUS 1: AHB BUS)
#define SUNXI_MMC_GCTL_DDR_MOD              BIT(10)     // DDR Mode Select (0: SDR 1:DDR)
#define SUNXI_MMC_GCTL_CDDBC_EN             BIT(8)      // Card Detect de-bounce Enable
#define SUNXI_MMC_GCTL_DMA_EN               BIT(5)      // DMA Global Enable
#define SUNXI_MMC_GCTL_INT_EN               BIT(4)      // Interrupt Global Enable
#define SUNXI_MMC_GCTL_DMA_RST              BIT(2)      // DMA Reset
#define SUNXI_MMC_GCTL_FIFO_RST             BIT(1)      // FIFO Reset
#define SUNXI_MMC_GCTL_SRST                 BIT(0)      // Software Reset

#define SUNXI_MMC_CKCR_CTRL                 BIT(17)     // Card Clock Output Control (Turn off clock when FSM IDLE)
#define SUNXI_MMC_CKCR_EN                   BIT(16)     // Card Clock Enable
#define SUNXI_MMC_CKCR_DIV                  (0xff<<0)   // Card Clock Divider

#define SUNXI_MMC_BWDR_8BIT                 (0x02)
#define SUNXI_MMC_BWDR_4BIT                 (0x01)
#define SUNXI_MMC_BWDR_1BIT                 (0x00)

#define SUNXI_MMC_CMDR_START                BIT(31)     // Start Command
#define SUNXI_MMC_CMDR_HOLD                 BIT(29)     // Use Hold
#define SUNXI_MMC_CMDR_VOL_SW               BIT(28)     // Voltage Switch
#define SUNXI_MMC_CMDR_BOOT_ABT             BIT(27)     // Boot Abort
#define SUNXI_MMC_CMDR_EXP_BOOT_ACK         BIT(26)     // Expect Boot Acknowledge
#define SUNXI_MMC_CMDR_BOOT_MOD             (0x03<<24)  // Boot Mode (0:normal command 1:Mandatory Boot operation 2:Alternate Boot operation)
#define SUNXI_MMC_CMDR_PRG_CLK              BIT(21)     // Change Clock
#define SUNXI_MMC_CMDR_SEND_INIT            BIT(15)     // Send Initialization
#define SUNXI_MMC_CMDR_STOP_ABT             BIT(14)     // Stop Abort Command
#define SUNXI_MMC_CMDR_WAIT_OVER            BIT(13)     // Wait Data Transfer Over
#define SUNXI_MMC_CMDR_AUTO_STOP            BIT(12)     // Send Stop CMD Automatically
#define SUNXI_MMC_CMDR_TRANS_MODE           BIT(11)     // Transfer Mode
#define SUNXI_MMC_CMDR_TRANS_DIR            BIT(10)     // Transfer Direction
#define SUNXI_MMC_CMDR_DATA_TRANS           BIT(9)      // Data Transfer
#define SUNXI_MMC_CMDR_CHK_RESP_CRC         BIT(8)      // Check Response CRC
#define SUNXI_MMC_CMDR_LONG_RESP            BIT(7)      // Response Type
#define SUNXI_MMC_CMDR_RESP_RCV             BIT(6)      // Response Receive
#define SUNXI_MMC_CMDR_IDX                  (0x1f<<0)   // CMD Index

#define SUNXI_MMC_RISR_CARD_REMOVE          BIT(31)
#define SUNXI_MMC_RISR_CARD_INSERT          BIT(30)
#define SUNXI_MMC_RISR_SDIO_INTERRUPT       BIT(16)
#define SUNXI_MMC_RISR_END_BIT_ERROR        BIT(15)
#define SUNXI_MMC_RISR_AUTO_COMMAND_DONE    BIT(14)
#define SUNXI_MMC_RISR_START_BIT_ERROR      BIT(13)
#define SUNXI_MMC_RISR_HARD_WARE_LOCKED     BIT(12)
#define SUNXI_MMC_RISR_FIFO_RUN_ERROR       BIT(11)
#define SUNXI_MMC_RISR_VOLTAGE_CHANGE_DONE	BIT(10)
#define SUNXI_MMC_RISR_DATA_TIMEOUT         BIT(9)
#define SUNXI_MMC_RISR_RESP_TIMEOUT         BIT(8)
#define SUNXI_MMC_RISR_DATA_CRC_ERROR       BIT(7)
#define SUNXI_MMC_RISR_RESP_CRC_ERROR       BIT(6)
#define SUNXI_MMC_RISR_DATA_RX_REQUEST      BIT(5)
#define SUNXI_MMC_RISR_DATA_TX_REQUEST      BIT(4)
#define SUNXI_MMC_RISR_DATA_OVER            BIT(3)
#define SUNXI_MMC_RISR_COMMAND_DONE         BIT(2)
#define SUNXI_MMC_RISR_RESP_ERROR           BIT(1)

#define SUNXI_MMC_STAR_DMA_REQ              BIT(31)
#define SUNXI_MMC_STAR_FIFO_LEVEL           (0x1f<<17)
#define SUNXI_MMC_STAR_RESP_IDX             (0x7f<<11)
#define SUNXI_MMC_STAR_FSM_BUSY             BIT(10)     // Data FSM Busy
#define SUNXI_MMC_STAR_CARD_BUSY            BIT(9)      // Card data busy
#define SUNXI_MMC_STAR_CARD_PRESENT         BIT(8)
#define SUNXI_MMC_STAR_FSM_STA              (0x0f<<4)   // Command FSM states
#define SUNXI_MMC_STAR_FIFO_FULL            BIT(3)      // FIFO Full
#define SUNXI_MMC_STAR_FIFO_EMPTY           BIT(2)      // FIFO Empty
#define SUNXI_MMC_STAR_FIFO_TX_LEVEL        BIT(1)      // FIFO TX Water Level flag
#define SUNXI_MMC_STAR_FIFO_RX_LEVEL        BIT(0)      // FIFO RX Water Level flag


#endif /* _DRIVER_MMC_SUNXI_H_ */
