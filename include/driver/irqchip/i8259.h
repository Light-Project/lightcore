/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IRQCHIP_8259_H_
#define _DRIVER_IRQCHIP_8259_H_

enum i8259_registers {
    I8259_CMD   = 0x00,
    I8259_DATA  = 0x01,
};

/********************************************************************/
/*      Mnemonic            value    meaning/usage                  */

#define ICW1_INIT	        0x10	/* Initialization - required!   */
#define ICW1_LEVEL	        0x08	/* Level triggered (edge) mode  */
#define ICW1_INTERVAL4	    0x04	/* Call address interval 4 (8)  */
#define ICW1_SINGLE	        0x02	/* Single (cascade) mode        */
#define ICW1_ICW4	        0x01	/* ICW4 needed                  */

#define ICW4_SFNM	        0x10	/* Special fully nested (not)   */
#define ICW4_BUF_MASTER	    0x0C	/* Buffered mode/master         */
#define ICW4_BUF_SLAVE	    0x08	/* Buffered mode/slave          */
#define ICW4_AUTO	        0x02	/* Auto (normal) EOI            */
#define ICW4_8086	        0x01	/* 8086/88 (MCS-80/85) mode     */

/* PIC1 bitdefs */
#define PIC1_IRQ0           (1<<0)  /* System Timer                                     */
#define PIC1_IRQ1           (1<<1)  /* PS2 Keyboard                                     */
#define PIC1_IRQ2           (1<<2)  /* Cascade Controller                               */
#define PIC1_IRQ3           (1<<3)  /* COM 2 and 4                                      */
#define PIC1_IRQ4           (1<<4)  /* COM 1 and 3                                      */
#define PIC1_IRQ5           (1<<5)  /* Sound, Parallel port 2                           */
#define PIC1_IRQ6           (1<<6)  /* Floppy                                           */
#define PIC1_IRQ7           (1<<7)  /* Parallel Port 1                                  */

/* PIC2 bitdefs */
#define PIC2_IRQ8           (1<<8)  /* Real-time clock                                  */
#define PIC2_IRQ9           (1<<9)  /* Redirected IRQ 2, Open, Network Available        */
#define PIC2_IRQ10          (1<<10) /* Open                                             */
#define PIC2_IRQ11          (1<<11) /* Open, SCSI, Video                                */
#define PIC2_IRQ12          (1<<12) /* PS2 Mouse                                        */
#define PIC2_IRQ13          (1<<13) /* Coprocessor                                      */
#define PIC2_IRQ14          (1<<14) /* Open, Primary hard drive, Hard drive controller  */
#define PIC2_IRQ15          (1<<15) /* Open, 2nd hard drive (secondary)                 */

#define PIC_EOI             0x20    /* End-of-interrupt command code */
#define PIC_READ_IRR        0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR        0x0b    /* OCW3 irq service next CMD read */

#endif
