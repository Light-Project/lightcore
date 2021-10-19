/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IRQCHIP_8259_H_
#define _DRIVER_IRQCHIP_8259_H_

#include <bits.h>

enum i8259_registers {
    I8259_CMD   = 0x00,
    I8259_DATA  = 0x01,
};

/****************************************************************************/
/*      Mnemonic                value       meaning/usage                   */

/**  Initialization Command Word (OCW)  **/
#define I8259_ICW1_VEC          BIT_RANGE(7, 5) /* Vector Addr (80 85 Mode) */
#define I8259_ICW1_INIT         BIT(4)  /* Initialization - Required!       */
#define I8259_ICW1_LEVEL        BIT(3)  /* Level Triggered (edge) Mode      */
#define I8259_ICW1_INTERVAL4    BIT(2)  /* Call Address Unterval 4 (8)      */
#define I8259_ICW1_SINGLE       BIT(1)  /* Single (Cascade) Mode            */
#define I8259_ICW1_ICW4         BIT(0)  /* ICW4 Needed                      */

#define I8259_ICW2_VEC          BIT_RANGE(7, 0) /* Vector Addr (86 88 mode) */

#define I8259_ICW3_SLVD         BIT_RANGE(2, 0) /* Slave Device Vector      */

#define I8259_ICW4_SFNM         BIT(4)  /* Special Fully Nested (not)       */
#define I8259_ICW4_BUFF         BIT(3)  /* Buffered Mode                    */
#define I8259_ICW4_SLAVE        BIT(2)  /* Master/Slave Mode                */
#define I8259_ICW4_AUTO         BIT(1)  /* Auto (normal) EOI                */
#define I8259_ICW4_8086         BIT(0)  /* 8086/88 (MCS-80/85) Mode         */

/**  Operation Control Word (OCW)  **/
#define I8259_OCW1_MASK         BIT_RANGE(7, 0) /* Interrupt Mask           */

#define I8259_OCW2_ROTITE       BIT(7)  /* ROTITE EOI                       */
#define I8259_OCW2_SL           BIT(6)  /* Specific EOI                     */
#define I8259_OCW2_EOI          BIT(5)  /* EOI command                      */
#define I8259_OCW2_VEC          BIT_RANGE(2, 0) /* Vector to EOI            */

#define I8259_OCW3_ESMM         BIT(6)  /*  */
#define I8259_OCW3_SMM          BIT(5)  /*  */
#define I8259_OCW3_INIT         BIT(3)  /*  */
#define I8259_OCW3_P            BIT(2)  /*  */
#define I8259_OCW3_RR           BIT(1)  /*  */
#define I8259_OCW3_RIS          BIT(0)  /*  */

#define I8259_IRQ_NR            8

#endif
