/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_IRQCHIP_IOAPIC_H_
#define _DRIVER_IRQCHIP_IOAPIC_H_

#include <bits.h>

enum ioapic_registers {
    IOAPIC_IND          = 0x00, /* RW: Index */
    IOAPIC_DAT          = 0x10, /* RW: Data */
    IOAPIC_EOIR         = 0x40, /* RW: EOI  */
};

enum ioapic_indexes {
    IOAPIC_ID           = 0x00, /* Identification */
    IOAPIC_VER          = 0x01, /* Version */
    IOAPIC_REDIR_BASE   = 0x10, /* Redirection Table */
};

#define IOAPIC_REDIR(index) (IOAPIC_REDIR_BASE + (index))

/************************************************************************************/
/*      Mnemonic                    value                 meaning/usage             */

/*
 * The APIC ID serves as a physical name of the APIC. The APIC bus arbitration ID for the
 * APIC is derived from its I/O APIC ID. This register is reset to 0 on power-up reset.
 */

#define IOAPIC_ID_PROG              BIT_RANGE(27, 24)
#define IOAPIC_ID_SCRATCHPAD        BIT(15)

/*
 * Each I/O APIC contains a hardwired Version Register that identifies different
 * implementation of APIC and their versions. The maximum redirection entry information
 * also is in this register, to let software know how many interrupt are supported by this
 * APIC.
 */

#define IOAPIC_VER_MAX_REDIR        BIT_RANGE(23, 16)
#define IOAPIC_VER_PRQ              BIT(15)
#define IOAPIC_VER_VERISON          BIT_RANGE(7, 0)

/*
 * The Redirection Table has a dedicated entry for each interrupt input pin. The
 * information in the Redirection Table is used to translate the interrupt manifestation on
 * the corresponding interrupt pin into an APIC message.
 */

#define IOAPIC_REDIR_DEST           BIT_RANGE(63, 56)   /* Destination */
#define IOAPIC_REDIR_EDID           BIT_RANGE(55, 48)   /* Extended Destination ID */
#define IOAPIC_REDIR_MASK           BIT(16)             /* Trigger Mode () */
#define IOAPIC_REDIR_TRIGGER        BIT(15)             /*  */
#define IOAPIC_REDIR_REMOTE         BIT(14)             /*  */
#define IOAPIC_REDIR_INPUT          BIT(13)             /*  */
#define IOAPIC_REDIR_DEL_STAT       BIT(12)             /*  */
#define IOAPIC_REDIR_DES_MODE       BIT(11)             /*  */
#define IOAPIC_REDIR_DEL_MODE       BIT_RANGE(10, 8)    /*  */
#define IOAPIC_REDIR_VECTOR         BIT_RANGE(7, 0)     /*  */

#endif /* _DRIVER_IRQCHIP_IOAPIC_H_ */
