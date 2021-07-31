/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _DRIVER_UART_8250_H_
#define _DRIVER_UART_8250_H_

enum UART8250_Registers
{
    UART8250_RBR       = 0,     // LCR.dlab=0: SRA: read-only
    UART8250_THR       = 0,     // LCR.dlab=0: Transmitter Holding Register
    UART8250_DLR_LSB   = 0,     // LCR.dlab=1: Divisor Latch Register LSB
    UART8250_DLR_MSB   = 1,     // LCR.dlab=1: Divisor Latch Register MSB
    UART8250_IER       = 1,     // Interrupt Enable Register
    UART8250_IIR       = 2,     // Interrupt Identification Register
    UART8250_FCR       = 2,     // FIFO Control Register
    UART8250_LCR       = 3,     // Line Control Register
    UART8250_MCR       = 4,     // MODEM Control Register
    UART8250_LSR       = 5,     // Line Status Register
    UART8250_MSR       = 6,     // MODEM Status Register
};

/****************************************************************************************/
/*      Mnemonic                    value        meaning/usage                          */

/*
 * This register enables the four types of UART interrupts. When enabled, each interrupt can individually activate the
 * interrupt (int_o) output signal. Each interrupt can individually be enabled/disabled by writing to the corresponding bit
 * in the IER. Disabling an interrupt prevents it from being indicated as active in the IIR and from activating the int_o
 * output signal. On the other hand, the UART status including Line Status and MODEM Status Registers are not affected
 * by IER setting.
 */

/* IER bitflag definitions */
#define UART8250_IER_MS_INT_EN     0x08        // MODEM Status Interrupt
#define UART8250_IER_RLS_INT_EN    0x04        // Receiver Line Status Interrupt
#define UART8250_IER_THRE_INT_EN   0x02        // Transmitter Holding Register Empty Interrupt
#define UART8250_IER_RDA_INT_EN    0x01        // Received Data Available Interrupt

/*
 * In order to minimize software overhead during data character transfers, the UART 16550 IP Core prioritizes interrupts
 * into four levels and records these in the Interrupt Identification Register. When the host reads the IIR, the IP Core only
 * indicates the highest priority pending interrupt. After clearing the source of the highest priority pending interrupt, IIR
 * updates to the next priority pending interrupt
 */

/* IIR bitflag definitions */
#define UART8250_IIR_FIFO          0x80        // 
#define UART8250_IIR_FIFO          0x40        // 
#define UART8250_IIR_TIMEOUT_INT   0x08        // Timeout Interrupt
#define UART8250_IIR_PRIO_RLS      0x06        // Receiver Line Status Interrupt
#define UART8250_IIR_PRIO_RDA      0x04        // Received Data Available Interrupt
#define UART8250_IIR_PRIO_THRE     0x02        // Transmitter Holding Register Empty Interrupt
#define UART8250_IIR_PRIO_MOD      0x00        // MODEM Status Interrupt
#define UART8250_IIR_PENDING       0x01        // Interrupt Pending

/*
 * This is a write only register at the same offset as the IIR, which is a read only register. This register is used to control the
 * XMIT FIFO and XCVR FIFO
 */

/* FCR bitflag definitions */
#define UART8250_FCR_LOOPBACK      0x10        // Provides a local loopback feature for diagnostic testing of the UART
#define UART8250_FCR_OUT2          0x08        // Auxiliary output 2 control
#define UART8250_FCR_OUT1          0x04        // Auxiliary output 1 control
#define UART8250_FCR_RTS           0x02        // Request to Send Output Control
#define UART8250_FCR_DTR           0x01

/*
 * This configures character length, number stop bits and parity bit
 */

/* LCR bitflag definitions */
#define UART8250_LCR_DLAB          0x80        // Divisor latch access bit
#define UART8250_LCR_SBRK          0x40        // Set Break
#define UART8250_LCR_STKP          0x20        // Stick Parity
#define UART8250_LCR_EPS           0x10        // Even Parity Select
#define UART8250_LCR_PEN           0x08        // Parity enable

#define UART8250_LCR_STB           0x04        // Number of stop Bits, off = 1, on = 1.5 or 2)

#define UART8250_LCR_WLS_MSK       0x03        // character length select mask
#define UART8250_LCR_WLS_8         0x03        // 8 bit character length
#define UART8250_LCR_WLS_7         0x02        // 7 bit character length
#define UART8250_LCR_WLS_6         0x01        // 6 bit character length
#define UART8250_LCR_WLS_5         0x00        // 5 bit character length

#define UART8250_MCR_TX_DFR         0x80
#define UART8250_MCR_DMA_EN         0x40
#define UART8250_MCR_LOOP           0x10        // Enable loopback test mode
#define UART8250_MCR_OUT2           0x08        // Out 2
#define UART8250_MCR_OUT1           0x04        // Out 1
#define UART8250_MCR_RTS            0x02        // RTS
#define UART8250_MCR_DTR            0x01        // DTR


/*
 * This provides status information to the Host concerning data transfer. This register is not affected by Interrupt Enable
 * Register. For example, if Receiver Line Status Interrupt is disabled, interrupt is not generated but the host can still read
 * the actual status of the transfer from this registe
 */

/* LSR bitflag definitions */
#define UART8250_LSR_ERR          0x80         // Error
#define UART8250_LSR_TEMT         0x40         // Xmitter empty
#define UART8250_LSR_THRE         0x20         // Xmit holding register empty
#define UART8250_LSR_BI           0x10         // Break
#define UART8250_LSR_OE           0x02         // Overrun
#define UART8250_LSR_PE           0x04         // Parity error
#define UART8250_LSR_OE           0x02         // Overrun
#define UART8250_LSR_DR           0x01         // Data ready

/*
 * This provides the current state of the control lines from the MODEM (or peripheral device) to the Host via the following
 * fields: ccts, cdsr, cri, cdcdi. In addition to this, the other four bits (dcts, ddsr, teri, ddcdi) of the MODEM Status Register
 * provide change information. These bits are set to a logic 1 when the corresponding control input from the MODEM
 * changes state. They are reset to logic 0 when the Host reads the MODEM Status Register.
 */

/* MSR bitflag definitions */
#define UART8250_MSR_DCD            0x80        // Complement of Data Carrier Detect input
#define UART8250_MSR_RI             0x40        // Complement of Ring Indicator input
#define UART8250_MSR_DSR            0x20        // Complement of Data Set Ready input
#define UART8250_MSR_CTS            0x10        // Complement of Clear to Send inpu
#define UART8250_MSR_DDCD           0x08        // Delta Data Carrier Detect Indicator
#define UART8250_MSR_TERI           0x04        // Trailing Edge of Ring indicator detector
#define UART8250_MSR_DDSR           0x02        // Delta Data Set Ready indicator
#define UART8250_MSR_DCTS           0x01        // Delta Clear to Send indicator

#endif  /* _DRIVER_UART_8250_H_ */
