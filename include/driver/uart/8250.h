/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_UART_8250_H_
#define _DRIVER_UART_8250_H_

#include <bits.h>

#ifndef UART8250_SHIFT
# define UART8250_SHIFT 0
#endif

enum uart8250_registers {
    UART8250_RBR        = 0 << UART8250_SHIFT,  /* (RO) LCR.dlab=0: Receive Register */
    UART8250_THR        = 0 << UART8250_SHIFT,  /* (WO) LCR.dlab=0: Transmitter Register */
    UART8250_DLL        = 0 << UART8250_SHIFT,  /* (RO) LCR.dlab=1: Divisor Latch Register LSB */
    UART8250_DLH        = 1 << UART8250_SHIFT,  /* (WO) LCR.dlab=1: Divisor Latch Register MSB */
    UART8250_IER        = 1 << UART8250_SHIFT,  /* (RW) LCR.dlab=0: Interrupt Enable Register */
    UART8250_IIR        = 2 << UART8250_SHIFT,  /* (RO) Interrupt Identification Register */
    UART8250_FCR        = 2 << UART8250_SHIFT,  /* (WO) FIFO Control Register */
    UART8250_LCR        = 3 << UART8250_SHIFT,  /* (RW) Line Control Register */
    UART8250_MCR        = 4 << UART8250_SHIFT,  /* (RW) MODEM Control Register */
    UART8250_LSR        = 5 << UART8250_SHIFT,  /* (RW) Line Status Register */
    UART8250_MSR        = 6 << UART8250_SHIFT,  /* (RW) MODEM Status Register */
};

/****************************************************************************************************/
/*      Mnemonic                        value               meaning/usage                           */

/*
 * This register enables the four types of UART interrupts. When enabled, each interrupt can individually activate the
 * interrupt (int_o) output signal. Each interrupt can individually be enabled/disabled by writing to the corresponding bit
 * in the IER. Disabling an interrupt prevents it from being indicated as active in the IIR and from activating the int_o
 * output signal. On the other hand, the UART status including Line Status and MODEM Status Registers are not affected
 * by IER setting.
 */
#define UART8250_IER_MS                 BIT(3)              /* MODEM Status Interrupt */
#define UART8250_IER_RLS                BIT(2)              /* Receiver Line Status Interrupt */
#define UART8250_IER_THRE               BIT(1)              /* Transmitter Holding Register Empty Interrupt */
#define UART8250_IER_RDA                BIT(0)              /* Received Data Available Interrupt */

/*
 * In order to minimize software overhead during data character transfers, the UART 16550 IP Core prioritizes interrupts
 * into four levels and records these in the Interrupt Identification Register. When the host reads the IIR, the IP Core only
 * indicates the highest priority pending interrupt. After clearing the source of the highest priority pending interrupt, IIR
 * updates to the next priority pending interrupt
 */
#define UART8250_IIR_FIFO               BIT(5)              /* FIFO Interrupt */
#define UART8250_IIR_TIMEOUT            BIT(3)              /* Timeout Interrupt */
#define UART8250_IIR_RLS                BIT_SHIFT(1, 3)     /* Receiver Line Status Interrupt */
#define UART8250_IIR_RDA                BIT_SHIFT(1, 2)     /* Received Data Available Interrupt */
#define UART8250_IIR_THRE               BIT_SHIFT(1, 1)     /* Transmitter Holding Register Empty Interrupt */
#define UART8250_IIR_MOD                BIT_SHIFT(1, 0)     /* MODEM Status Interrupt */
#define UART8250_IIR_PENDING            BIT(0)              /* Interrupt Pending */

/*
 * This is a write only register at the same offset as the IIR, which is a read only register. This register is used to control the
 * XMIT FIFO and XCVR FIFO
 */
#define UART8250_FCR_TRIGGER_14         BIT_SHIFT(6, 3)     /* Mask for Trigger Set at 14 */
#define UART8250_FCR_TRIGGER_8          BIT_SHIFT(6, 2)     /* Mask for Trigger Set at 8 */
#define UART8250_FCR_TRIGGER_4          BIT_SHIFT(6, 1)     /* Mask for Trigger Set at 4 */
#define UART8250_FCR_TRIGGER_1          BIT_SHIFT(6, 0)     /* Mask for Trigger Set at 1 */
#define UART8250_FCR_DMA_SELECT         BIT(3)              /* For DMA applications */
#define UART8250_FCR_CLEAR_XMIT         BIT(2)              /* Clear the XMIT FIFO */
#define UART8250_FCR_CLEAR_RCVR         BIT(1)              /* Clear the RCVR FIFO */
#define UART8250_FCR_FIFO_EN            BIT(0)              /* Enable the FIFO */

/*
 * This configures character length, number stop bits and parity bit.
 */
#define UART8250_LCR_DLAB               BIT(7)              /* Divisor latch access bit */
#define UART8250_LCR_SBRK               BIT(6)              /* Set Break Control */
#define UART8250_LCR_STKP               BIT(5)              /* Stick Parity */
#define UART8250_LCR_EPS                BIT(4)              /* Even Parity Select */
#define UART8250_LCR_PEN                BIT(3)              /* Parity enable */
#define UART8250_LCR_STB                BIT(2)              /* Number of stop Bits (0: 1bits 1: 2bits) */
#define UART8250_LCR_WLS_MASK           BIT_RANGE(1, 0)     /* Transmit Wordlength */
#define UART8250_LCR_WLS_8              BIT_SHIFT(0, 3)     /* 8 Bit Character Length */
#define UART8250_LCR_WLS_7              BIT_SHIFT(0, 2)     /* 7 Bit Character Length */
#define UART8250_LCR_WLS_6              BIT_SHIFT(0, 1)     /* 6 Bit Character Length */
#define UART8250_LCR_WLS_5              BIT_SHIFT(0, 0)     /* 5 Bit Character Length */

#define UART8250_MCR_TX_DFR             BIT(7)              /*  */
#define UART8250_MCR_DMA_EN             BIT(6)              /*  */
#define UART8250_MCR_LOOP               BIT(4)              /* Enable loopback test mode */
#define UART8250_MCR_OUT2               BIT(3)              /* Complement Out 2 Output */
#define UART8250_MCR_OUT1               BIT(2)              /* Complement Out 1 Output */
#define UART8250_MCR_RTS                BIT(1)              /* Complement RTS Output */
#define UART8250_MCR_DTR                BIT(0)              /* Complement DTR Output */

/*
 * This provides status information to the Host concerning data transfer. This register is not affected by Interrupt Enable
 * Register. For example, if Receiver Line Status Interrupt is disabled, interrupt is not generated but the host can still read
 * the actual status of the transfer from this register.
 */
#define UART8250_LSR_ERR                BIT(7)              /* FIFO Error */
#define UART8250_LSR_TEMT               BIT(6)              /* Transmitter Empty */
#define UART8250_LSR_THRE               BIT(5)              /* Transmit Holding Register Empty */
#define UART8250_LSR_BI                 BIT(4)              /* Break Interrupt Indicator */
#define UART8250_LSR_FE                 BIT(3)              /* Frame Error Indicator */
#define UART8250_LSR_PE                 BIT(2)              /* Parity Error Indicator */
#define UART8250_LSR_OE                 BIT(1)              /* Overrun Error Indicator */
#define UART8250_LSR_DR                 BIT(0)              /* Receiver Data Ready */

/*
 * This provides the current state of the control lines from the MODEM (or peripheral device) to the Host via the following
 * fields: ccts, cdsr, cri, cdcdi. In addition to this, the other four bits (dcts, ddsr, teri, ddcdi) of the MODEM Status Register
 * provide change information. These bits are set to a logic 1 when the corresponding control input from the MODEM
 * changes state. They are reset to logic 0 when the Host reads the MODEM Status Register.
 */
#define UART8250_MSR_DCD                BIT(7)              /* Complement of Data Carrier Detect Input */
#define UART8250_MSR_RI                 BIT(6)              /* Complement of Ring Indicator Input */
#define UART8250_MSR_DSR                BIT(5)              /* Complement of Data Set Ready Input */
#define UART8250_MSR_CTS                BIT(4)              /* Complement of Clear to Send Input */
#define UART8250_MSR_DDCD               BIT(3)              /* Delta Data Carrier Detect Indicator */
#define UART8250_MSR_TERI               BIT(2)              /* Trailing Edge of Ring Indicator detector */
#define UART8250_MSR_DDSR               BIT(1)              /* Delta Data Set Ready Indicator */
#define UART8250_MSR_DCTS               BIT(0)              /* Delta Clear to Send Indicator */

#endif /* _DRIVER_UART_8250_H_ */
