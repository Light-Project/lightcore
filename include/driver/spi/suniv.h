/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_SPI_SUNIV_H_
#define _DRIVER_SPI_SUNIV_H_

#include <bits.h>

enum suniv_spi_registers {
    SUNIV_SPI_GCR   = 0x04,
    SUNIV_SPI_TCR   = 0x08,
    SUNIV_SPI_IER   = 0x10,
    SUNIV_SPI_ISR   = 0x14,
    SUNIV_SPI_FCR   = 0x18,
    SUNIV_SPI_FSR   = 0x1c,
    SUNIV_SPI_WCR   = 0x20,
    SUNIV_SPI_CCR   = 0x24,
    SUNIV_SPI_MBC   = 0x30,
    SUNIV_SPI_MTC   = 0x34,
    SUNIV_SPI_BCC   = 0x38,
    SUNIV_SPI_TXD   = 0x200,
    SUNIV_SPI_RXD   = 0x300,
};

/****************************************************************************************/
/*      Mnemonic                    value        meaning/usage                          */

#define SUNIV_SPI_GCR_SRST          BIT(31)     // Soft reset
#define SUNIV_SPI_GCR_TPEN          BIT(7)      // Transmit Pause Enable
#define SUNIV_SPI_GCR_MODE          BIT(1)      // SPI Mode Select (0:Slave 1:Master)
#define SUNIV_SPI_GCR_EN            BIT(0)      // SPI Module Enable Control

#define SUNIV_SPI_TCR_XCH           BIT(31)     // Exchange Burst
#define SUNIV_SPI_TCR_SDM           BIT(13)     // Master Sample Data Mode
#define SUNIV_SPI_TCR_FBS           BIT(12)     // First Transmit Bit Select
#define SUNIV_SPI_TCR_SDC           BIT(11)     // Master Sample Data Control
#define SUNIV_SPI_TCR_RPSM          BIT(10)     // Rapids mode select
#define SUNIV_SPI_TCR_DDB           BIT(9)      // Dummy Burst Type
#define SUNIV_SPI_TCR_DHB           BIT(8)      // Discard Hash Burst
#define SUNIV_SPI_TCR_SS_VAL        BIT(7)      // SS Level
#define SUNIV_SPI_TCR_SS_OWN        BIT(6)      // SS Output Owner Select
#define SUNIV_SPI_TCR_SS_SEL3       (3<<4)      // SPI Chip Select: SS3
#define SUNIV_SPI_TCR_SS_SEL2       (2<<4)      // SPI Chip Select: SS2
#define SUNIV_SPI_TCR_SS_SEL1       (1<<4)      // SPI Chip Select: SS1
#define SUNIV_SPI_TCR_SS_SEL0       (0<<4)      // SPI Chip Select: SS0
#define SUNIV_SPI_TCR_SSCCTL        BIT(3)      // Selects the output wave form for the SPI_SSx signal
#define SUNIV_SPI_TCR_SPOL          BIT(2)      // SPI Chip Select Signal Polarity Control
#define SUNIV_SPI_TCR_CPOL          BIT(1)      // SPI Clock Polarity Control
#define SUNIV_SPI_TCR_CPHA          BIT(0)      // SPI Clock/Data Phase Control

#define SUNIV_SPI_INT_IER_SS        BIT(13)     // SSI Interrupt Enable
#define SUNIV_SPI_INT_TC            BIT(12)     // Transfer Completed Interrupt Enable
#define SUNIV_SPI_INT_TF_UDR        BIT(11)     // TX FIFO under run Interrupt Enable
#define SUNIV_SPI_INT_TF_OVF        BIT(10)     // TX FIFO Overflow Interrupt Enable
#define SUNIV_SPI_INT_RF_UDR        BIT(9)      // RX FIFO under run Interrupt Enable
#define SUNIV_SPI_INT_RF_OVF        BIT(8)      // RX FIFO Overflow Interrupt Enable
#define SUNIV_SPI_INT_TF_FUL        BIT(6)      // TX FIFO Full Interrupt Enable
#define SUNIV_SPI_INT_TX_EMP        BIT(5)      // TX FIFO Empty Interrupt Enable
#define SUNIV_SPI_INT_TX_ERQ        BIT(4)      // TX FIFO Empty Request Interrupt Enable
#define SUNIV_SPI_INT_RF_FUL        BIT(2)      // RX FIFO Full Interrupt Enable
#define SUNIV_SPI_INT_RX_EMP        BIT(1)      // RX FIFO Empty Interrupt Enable
#define SUNIV_SPI_INT_RF_RDY        BIT(0)      // RX FIFO Request Request Interrupt Enable

#define SUNIV_SPI_FCR_TX_FIFO_RST   BIT(31)     //
#define SUNIV_SPI_FCR_TF_TEST_ENB   (0x03<<26)  //
#define SUNIV_SPI_FCR_TX_FIFO_SIZE  BIT(31)     //
#define SUNIV_SPI_FCR_TF_DRQ_EN     BIT(24)     //
#define SUNIV_SPI_FCR_TX_TRIG_LEVEL (0x03<<16)  //
#define SUNIV_SPI_FCR_RX_FIFO_RST   BIT(15)     //
#define SUNIV_SPI_FCR_RX_FIFO_TEST  BIT(14)     //
#define SUNIV_SPI_FCR_RX_FIFO_SIZE  (0x03<<10)  //
#define SUNIV_SPI_FCR_RX_DMA_MODE   BIT(19)     //
#define SUNIV_SPI_FCR_RX_DRQ_EN     BIT(8)      //
#define SUNIV_SPI_FCR_RX_TRIG_LEVEL (0xff<<0)   //

#define SUNIV_SPI_FSR_TB_WR         (0x07<<28)
#define SUNIV_SPI_FSR_TB_CNT        (0x3f<<16)
#define SUNIV_SPI_FSR_TF_CNT        (0x3f<<16)
#define SUNIV_SPI_FSR_RB_WR         BIT(15)
#define SUNIV_SPI_FSR_RB_CNT        (0x07<<12)
#define SUNIV_SPI_FSR_RF_CNT        (0xff<<0)

#define SUNIV_SPI_CCR_DRS           BIT(12)

#endif  /* _DRIVER_SPI_SUNIV_H_ */
