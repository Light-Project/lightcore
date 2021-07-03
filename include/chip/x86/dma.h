/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/dma.h
 *  x86 dma register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-14      first version 
 * 
 */
 
#ifndef _CHIP_X86_DMA_H_
#define _CHIP_X86_DMA_H_ 

enum DMA_Registers
{
    /* DMA1 io port */
    DMA1_CH0_SAR        = 0x00, // Channel 0 start address register
    DMA1_CH0_CR         = 0x01, // Channel 0 count register
    DMA1_CH1_SAR        = 0x02, // Channel 1 start address register
    DMA1_CH1_CR         = 0x03, // Channel 1 count register
    DMA1_CH2_SAR        = 0x04, // Channel 2 start address register
    DMA1_CH2_CR         = 0x05, // Channel 2 count register
    DMA1_CH3_SAR        = 0x06, // Channel 3 start address register
    DMA1_CH3_CR         = 0x07, // Channel 3 count register
    DMA1_STATUS         = 0x08, // DMA 1 Status Register (read)
    DMA1_CMD            = 0x08, // DMA 1 Command Register (write)
    DMA1_REQUEST        = 0x09, // DMA 1 Request Register 
    DMA1_SMASK          = 0x0a, // DMA 1 Single Channel Mask Register 
    DMA1_MODE           = 0x0b, // DMA 1 Mode Register 
    DMA1_TRIGGER        = 0x0c, // DMA 1 Flip-Flop Reset Register 
    DMA1_MID            = 0x0d, // DMA 1 Intermediate Register 
    DMA1_MASTER_RST     = 0x0d, // DMA 1 Master Reset Register 
    DMA1_MASK_RST       = 0x0e, // DMA 1 Mask Reset Register 
    DMA1_MMASK          = 0x0f, // DMA 1 MultiChannel Mask Register
    /* DMA2 io port */
    DMA2_CH4_SAR        = 0xc0, // Channel 4 start address register
    DMA2_CH4_CR         = 0xc2, // Channel 4 count register
    DMA2_CH5_SAR        = 0xc4, // Channel 5 start address register
    DMA2_CH5_CR         = 0xc6, // Channel 5 count register
    DMA2_CH6_SAR        = 0xc8, // Channel 6 start address register
    DMA2_CH6_CR         = 0xca, // Channel 6 count register
    DMA2_CH7_SAR        = 0xcc, // Channel 7 start address register
    DMA2_CH7_CR         = 0xce, // Channel 7 count register
    DMA2_STATUS         = 0xd0, // DMA 2 Status Register (read)
    DMA2_CMD            = 0xd0, // DMA 2 Command Register (write)
    DMA2_REQUEST        = 0xd2, // DMA 2 Request Register 
    DMA2_SMASK          = 0xd4, // DMA 2 Single Channel Mask Register 
    DMA2_MODE           = 0xd6, // DMA 2 Mode Register 
    DMA2_TRIGGER        = 0xd8, // DMA 2 Flip-Flop Reset Register 
    DMA2_MID            = 0xda, // DMA 2 Intermediate Register 
    DMA2_MASTER_RST     = 0xda, // DMA 2 Master Reset Register 
    DMA2_MASK_RST       = 0xdc, // DMA 2 Mask Reset Register 
    DMA2_MMASK          = 0xde, // DMA 2 MultiChannel Mask Register
    /* Page Address io port */
    DMA_PAGE_CH0        = 0x87, // Channel 0 Page Address Register (unusable)
    DMA_PAGE_CH1        = 0x83, // Channel 1 Page Address Register 
    DMA_PAGE_CH2        = 0x81, // Channel 2 Page Address Register 
    DMA_PAGE_CH3        = 0x82, // Channel 3 Page Address Register 
    DMA_PAGE_CH4        = 0x8F, // Channel 4 Page Address Register (unusable) 
    DMA_PAGE_CH5        = 0x8B, // Channel 5 Page Address Register 
    DMA_PAGE_CH6        = 0x89, // Channel 6 Page Address Register 
    DMA_PAGE_CH7        = 0x8A, // Channel 7 Page Address Register 
};

/*************************************************************************************/
/*      Mnemonic                 value        meaning/usage                          */

/* STATUS bitflag definitions */
#define DMA_STATUS_REQ3         0x80        // DMA Channel 3 Request Pending
#define DMA_STATUS_REQ2         0x40        // DMA Channel 2 Request Pending
#define DMA_STATUS_REQ1         0x20        // DMA Channel 1 Request Pending 
#define DMA_STATUS_REQ0         0x10        // DMA Channel 0 Request Pending
#define DMA_STATUS_TC3          0x08        // DMA Channel 3 Transfer Complete
#define DMA_STATUS_TC2          0x04        // DMA Channel 2 Transfer Complete
#define DMA_STATUS_TC1          0x02        // DMA Channel 1 Transfer Complete
#define DMA_STATUS_TC0          0x01        // DMA Channel 0 Transfer Complete

/* CMD bitflag definitions */
#define DMA_CMD_DACKP           0x80        // 
#define DMA_CMD_DRQP            0x40        // 
#define DMA_CMD_EXTW            0x20        // 
#define DMA_CMD_PRIO            0x10        // 
#define DMA_CMD_COMP            0x08        // 
#define DMA_CMD_COND            0x04        // 
#define DMA_CMD_ADHE            0x02        // 
#define DMA_CMD_MMT             0x01        // Memory-to-memory

/* Mode bitflag definitions */
#define DMA_MODE_MOD_CASCADE    0xc0        // Mode select: Cascade Mode (use to cascade another DMA controller)
#define DMA_MODE_MOD_BLOCK      0x80        // Mode select: Block DMA Transfer
#define DMA_MODE_MOD_SINGLE     0x40        // Mode select: Single DMA Transfer
#define DMA_MODE_MOD_DEMAND     0x00        // Mode select: Transfer on Demand

#define DMA_MODE_DOWN           0x20        // Reverses the memory order of the data
#define DMA_MODE_AUTO           0x10        // Cyclic transmission

#define DMA_MODE_TRA_TOPER      0x08        // Transfer type: Peripheral is reading from memory
#define DMA_MODE_TRA_TOMEM      0x04        // Transfer type: Peripheral is writing to memory
#define DMA_MODE_TRA_TEST       0x00        // Transfer type: runs a self test of the controller

#define DMA_MODE_SEL_CH3        0x03        // Channel select: CH3
#define DMA_MODE_SEL_CH2        0x02        // Channel select: CH2
#define DMA_MODE_SEL_CH1        0x01        // Channel select: CH1
#define DMA_MODE_SEL_CH0        0x00        // Channel select: CH0

/* SMASK bitflag definitions */
#define DMA_SMASK_MASKON        0x04        // Single Channel Mask Enable
#define DMA_SMASK_SEL3          0x03        // Channel mask: CH3
#define DMA_SMASK_SEL2          0x02        // Channel mask: CH2
#define DMA_SMASK_SEL1          0x01        // Channel mask: CH1
#define DMA_SMASK_SEL0          0x00        // Channel mask: CH0

/* MMASK bitflag definitions */
#define DMA_MMASK_MASK3         0x08        // Channel mask: CH3
#define DMA_MMASK_MASK2         0x04        // Channel mask: CH2
#define DMA_MMASK_MASK1         0x02        // Channel mask: CH1
#define DMA_MMASK_MASK0         0x01        // Channel mask: CH0

#endif
