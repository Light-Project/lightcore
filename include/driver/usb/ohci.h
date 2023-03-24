/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_USB_OHCI_H_
#define _DRIVER_USB_OHCI_H_

#include <bits.h>

enum ohci_registers {
    /* control and status registers */
    OHCI_REVISION           = 0x00,
    OHCI_CONTROL            = 0x04,
    OHCI_CMDSTATUS          = 0x08,
    OHCI_INTRSTATUS         = 0x0c,
    OHCI_INTRENABLE         = 0x10,
    OHCI_INTRDISABLE        = 0x14,

    /* memory pointers */
    OHCI_HCCA               = 0x18,
    OHCI_ED_PERIODCURRENT   = 0x1c,
    OHCI_ED_CONTROLHEAD     = 0x20,
    OHCI_ED_CONTROLCURRENT  = 0x24,
    OHCI_ED_BULKHEAD        = 0x28,
    OHCI_ED_BULKCURRENT     = 0x2c,
    OHCI_DONEHEAD           = 0x30,

    /* frame counters */
    OCHI_FMINTERVAL         = 0x34,
    OCHI_FMREMAINING        = 0x38,
    OCHI_FMNUMBER           = 0x3c,
    OCHI_PERIODICSTART      = 0x40,
    OCHI_LSTHRESH           = 0x44,

    /* Root hub ports */
    OCHI_ROOTHUB_A          = 0x48,
    OCHI_ROOTHUB_B          = 0x4c,
    OCHI_ROOTHUB_STATUS     = 0x50,
    OCHI_ROOTHUB_PSTATUS    = 0x54,
};

enum ohci_status {
    OHCI_ENOERR,        /* No Error */
    OHCI_ECRC,          /* CRC Error */
    OHCI_EBIT,          /* Bit Stuff */
    OHCI_EDATA,         /* Data Togg */
    OHCI_ESTALL,        /* Stall */
};

/*******************************************************************************/
/*      Mnemonic             value        meaning/usage                        */

/* control bitflag definitions */
#define OHCI_CTRL_CBSR      (3 << 0)    /* control/bulk service ratio */
#define OHCI_CTRL_PLE       (1 << 2)    /* periodic list enable */
#define OHCI_CTRL_IE        (1 << 3)    /* isochronous enable */
#define OHCI_CTRL_CLE       (1 << 4)    /* control list enable */
#define OHCI_CTRL_BLE       (1 << 5)    /* bulk list enable */
#define OHCI_CTRL_HCFS      (3 << 6)    /* host controller functional state */
#define OHCI_CTRL_IR        (1 << 8)    /* interrupt routing */
#define OHCI_CTRL_RWC       (1 << 9)    /* remote wakeup connected */
#define OHCI_CTRL_RWE       (1 << 10)   /* remote wakeup enable */

/* cmdstatus bitflag definitions */
#define OHCI_CMDSTAT_HCR    (1 << 0)    /* host controller reset */
#define OHCI_CMDSTAT_CLF    (1 << 1)    /* control list filled */
#define OHCI_CMDSTAT_BLF    (1 << 2)    /* bulk list filled */
#define OHCI_CMDSTAT_OCR    (1 << 3)    /* ownership change request */
#define OHCI_CMDSTAT_SOC    (3 << 16)   /* scheduling overrun count */

/* intrstatus/intrenable/intrdisable bitflag definitions */
#define OHCI_INTR_SO        (1 << 0)    /* scheduling overrun */
#define OHCI_INTR_WDH       (1 << 1)    /* writeback of done_head */
#define OHCI_INTR_SF        (1 << 2)    /* start frame */
#define OHCI_INTR_RD        (1 << 3)    /* resume detect */
#define OHCI_INTR_UE        (1 << 4)    /* unrecoverable error */
#define OHCI_INTR_FNO       (1 << 5)    /* frame number overflow */
#define OHCI_INTR_RHSC      (1 << 6)    /* root hub status change */
#define OHCI_INTR_OC        (1 << 30)   /* ownership change */
#define OHCI_INTR_MIE       (1 << 31)   /* master interrupt enable */

/* roothub status bitflag definitions */
#define OHCI_RH_HS_LPS      (1 << 0)    /* local power status */
#define OHCI_RH_HS_OCI	    (1 << 1)    /* over current indicator */
#define OHCI_RH_HS_DRWE	    (1 << 15)   /* device remote wakeup enable */
#define OHCI_RH_HS_LPSC	    (1 << 16)   /* local power status change */
#define OHCI_RH_HS_OCIC	    (1 << 17)   /* over current indicator change */
#define OHCI_RH_HS_CRWE	    (1 << 31)   /* clear remote wakeup enable */

/* roothub portstatus bitflag definitions */
#define OHCI_RH_PS_CCS      (1 << 0)    /* current connect status */
#define OHCI_RH_PS_PES      (1 << 1)    /* port enable status*/
#define OHCI_RH_PS_PSS      (1 << 2)    /* port suspend status */
#define OHCI_RH_PS_POCI     (1 << 3)    /* port over current indicator */
#define OHCI_RH_PS_PRS      (1 << 4)    /* port reset status */
#define OHCI_RH_PS_PPS      (1 << 8)    /* port power status */
#define OHCI_RH_PS_LSDA     (1 << 9)    /* low speed device attached */
#define OHCI_RH_PS_CSC      (1 << 16)   /* connect status change */
#define OHCI_RH_PS_PESC     (1 << 17)   /* port enable status change */
#define OHCI_RH_PS_PSSC     (1 << 18)   /* port suspend status change */
#define OHCI_RH_PS_OCIC     (1 << 19)   /* over current indicator change */
#define OHCI_RH_PS_PRSC     (1 << 20)   /* port reset status change */

/* roothub a bitflag definitions */
#define	OHCI_RH_A_NDP       (0xff << 0)     /* number of downstream ports */
#define	OHCI_RH_A_PSM       (1 << 8)        /* power switching mode */
#define	OHCI_RH_A_NPS       (1 << 9)        /* no power switching */
#define	OHCI_RH_A_DT        (1 << 10)       /* device type (mbz) */
#define	OHCI_RH_A_OCPM      (1 << 11)       /* over current protection mode */
#define	OHCI_RH_A_NOCP      (1 << 12)       /* no over current protection */
#define	OHCI_RH_A_POTPGT    (0xff << 24)    /* power on to power good time */

/* roothub b bitflag definitions */
#define OHCI_RH_B_DR        (0xffff	<< 0)   /* device removable flags */
#define OHCI_RH_B_PPCM      (0xffff	<< 16)  /* port power control mask */

#endif /* _DRIVER_USB_OHCI_H_ */
