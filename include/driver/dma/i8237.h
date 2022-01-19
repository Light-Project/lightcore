/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DMA_I8237_H_
#define _DRIVER_DMA_I8237_H_

#include <bits.h>

enum i8237_registers {
    I8237_CH0_SAR       = 0x00, /* (RW) Channel 0 start address register */
    I8237_CH0_CR        = 0x01, /* (RW) Channel 0 count register */
    I8237_CH1_SAR       = 0x02, /* (RW) Channel 1 start address register */
    I8237_CH1_CR        = 0x03, /* (RW) Channel 1 count register */
    I8237_CH2_SAR       = 0x04, /* (RW) Channel 2 start address register */
    I8237_CH2_CR        = 0x05, /* (RW) Channel 2 count register */
    I8237_CH3_SAR       = 0x06, /* (RW) Channel 3 start address register */
    I8237_CH3_CR        = 0x07, /* (RW) Channel 3 count register */
    I8237_STATUS        = 0x08, /* (RO) I8237 Status Register */
    I8237_CMD           = 0x08, /* (WO) I8237 Command Register */
    I8237_REQUEST       = 0x09, /* (WO) I8237 Request Register */
    I8237_SMASK         = 0x0a, /* (WO) I8237 Single Channel Mask Register */
    I8237_MODE          = 0x0b, /* (WO) I8237 Mode Register */
    I8237_TRIGGER       = 0x0c, /* (WO) I8237 Flip-Flop Reset Register */
    I8237_MID           = 0x0d, /* (WO) I8237 Intermediate Register */
    I8237_MASTER_RST    = 0x0d, /* (WO) I8237 Master Reset Register */
    I8237_MASK_RST      = 0x0e, /* (WO) I8237 Mask Reset Register */
    I8237_MMASK         = 0x0f, /* (RW) I8237 MultiChannel Mask Register */
};

#define I8237_CH_SAR(ch)  (I8237_CH0_SAR + (ch * 2))
#define I8237_CH_CR(ch)   (I8237_CH0_CR  + (ch * 2))

/****************************************************************************************/
/*      Mnemonic                value              meaning/usage                        */

#define I8237_STATUS_REQ3       BIT(7)          /* DMA Channel 3 Request Pending */
#define I8237_STATUS_REQ2       BIT(6)          /* DMA Channel 2 Request Pending */
#define I8237_STATUS_REQ1       BIT(5)          /* DMA Channel 1 Request Pending */
#define I8237_STATUS_REQ0       BIT(4)          /* DMA Channel 0 Request Pending */
#define I8237_STATUS_TC3        BIT(3)          /* DMA Channel 3 Transfer Complete */
#define I8237_STATUS_TC2        BIT(2)          /* DMA Channel 2 Transfer Complete */
#define I8237_STATUS_TC1        BIT(1)          /* DMA Channel 1 Transfer Complete */
#define I8237_STATUS_TC0        BIT(0)          /* DMA Channel 0 Transfer Complete */

#define I8237_CMD_DACKP         BIT(7)          //
#define I8237_CMD_DRQP          BIT(6)          //
#define I8237_CMD_EXTW          BIT(5)          //
#define I8237_CMD_PRIO          BIT(4)          //
#define I8237_CMD_COMP          BIT(3)          //
#define I8237_CMD_COND          BIT(2)          //
#define I8237_CMD_ADHE          BIT(1)          //
#define I8237_CMD_MMT           BIT(0)          // Memory-to-memory

#define I8237_MODE_MOD_MASK     BIT_RANGE(7, 6) // Mode Select:
#define I8237_MODE_MOD_CASCADE  BIT_SHIFT(6, 3) // Cascade Mode (use to cascade another DMA controller)
#define I8237_MODE_MOD_BLOCK    BIT_SHIFT(6, 2) // Block DMA Transfer
#define I8237_MODE_MOD_SINGLE   BIT_SHIFT(6, 1) // Single DMA Transfer
#define I8237_MODE_MOD_DEMAND   BIT_SHIFT(6, 0) // Transfer on Demand
#define I8237_MODE_DOWN         BIT(5)          // Reverses the memory order of the data
#define I8237_MODE_AUTO         BIT(4)          // Cyclic transmission
#define I8237_MODE_TRA_MASK     BIT_RANGE(3, 2) // Transfer Type:
#define I8237_MODE_TRA_TOPER    BIT_SHIFT(2, 2) //   Peripheral is reading from memory
#define I8237_MODE_TRA_TOMEM    BIT_SHIFT(2, 1) //   Peripheral is writing to memory
#define I8237_MODE_TRA_TEST     BIT_SHIFT(2, 0) //   runs a self test of the controller
#define I8237_MODE_SEL_MASK     BIT_RANGE(1, 0) // Channel Select:
#define I8237_MODE_SEL_CH3      BIT_SHIFT(0, 3) //   Channel 3
#define I8237_MODE_SEL_CH2      BIT_SHIFT(0, 2) //   Channel 2
#define I8237_MODE_SEL_CH1      BIT_SHIFT(0, 1) //   Channel 1
#define I8237_MODE_SEL_CH0      BIT_SHIFT(0, 0) //   Channel 0

#define I8237_SMASK_MASKON      BIT(2)          /* Single Channel Mask Enable */
#define I8237_SMASK_MASK        BIT_RANGE(1, 0) /* Channel Mask */
#define I8237_SMASK_SEL3        BIT_SHIFT(0, 3) /* Channel 3 */
#define I8237_SMASK_SEL2        BIT_SHIFT(0, 2) /* Channel 2 */
#define I8237_SMASK_SEL1        BIT_SHIFT(0, 1) /* Channel 1 */
#define I8237_SMASK_SEL0        BIT_SHIFT(0, 0) /* Channel 0 */

#define I8237_MMASK_MASK        BIT_RANGE(3, 0) /* Channel Mask */
#define I8237_MMASK_CH3         BIT(3)          /* Channel 3 */
#define I8237_MMASK_CH2         BIT(2)          /* Channel 2 */
#define I8237_MMASK_CH1         BIT(1)          /* Channel 1 */
#define I8237_MMASK_CH0         BIT(0)          /* Channel 0 */

#endif
