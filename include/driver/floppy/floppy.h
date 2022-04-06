/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_FLOPPY_FLOPPY_H_
#define _DRIVER_FLOPPY_FLOPPY_H_

/*
 * The 82077AA floppy disk controller has completely integrated all of the logic required for floppy disk control.
 * The 82077AA, a 24 MHz crystal, a resistor package and a device chip select implements a PC AT or PS/2
 * solution. All programmable options default to compatible values. The dual PLL data separator has better
 * performance than most board level/discrete PLL implementations. The FIFO allows better system perform-
 * ance in multi-master systems (e.g. PS/2, EISA). The 82077AA is available in three versionsÐ82077AA-5,
 * 82077AA and 82077AA-1. 82077AA-1 has all features listed in this data sheet. It supports both tape drives and
 * 4 Mb floppy drives. The 82077AA supports 4 Mb floppy drives and is capable of operation at all data rates
 * through 1 Mbps. The 82077AA-5 supports 500/300/250 Kbps data rates for high and low density floppy
 * drives.
 */

enum floppy_registers {
    FLOPPY_SRA                      = 0x00, /* (RO): Status Register A */
    FLOPPY_SRB                      = 0x01, /* (RO): Status Register B */
    FLOPPY_DOR                      = 0x02, /* (RW): Digital Output_Register */
    FLOPPY_TDR                      = 0x03, /* (RW): Tape Drive Register */
    FLOPPY_MSR                      = 0x04, /* (RO): Main Status Register */
    FLOPPY_DSR                      = 0x04, /* (WO): Datarate Select Register */
    FLOPPY_FIFO                     = 0x05, /* (RW): Data Fifo */
    FLOPPY_DIR                      = 0x07, /* (RO): Digital Input Register */
    FLOPPY_CCR                      = 0x07, /* (WO): Configuration Control Register */
};

enum floppy_commands {
    FLOPPY_CMD_READ_TRACK           = 0x02, /* generates IRQ6 */
    FLOPPY_CMD_SPECIFY              = 0x03, /* set drive parameters */
    FLOPPY_CMD_SENSE_DRIVE_STATUS   = 0x04, /* read ST3 */
    FLOPPY_CMD_WRITE_DATA           = 0x45, /* write to the disk */
    FLOPPY_CMD_READ_DATA            = 0x66, /* read from the disk */
    FLOPPY_CMD_RECALIBRATE          = 0x07, /* seek to cylinder 0 */
    FLOPPY_CMD_SENSE_INTERRUPT      = 0x08, /* ack IRQ6, get status of last command */
    FLOPPY_CMD_WRITE_DELETED_DATA   = 0x09, /* */
    FLOPPY_CMD_READ_ID              = 0x4a, /* generates IRQ6 */
    FLOPPY_CMD_READ_DELETED_DATA    = 0x0c, /* */
    FLOPPY_CMD_FORMAT_TRACK         = 0x4d, /* */
    FLOPPY_CMD_DUMPREG              = 0x0e, /* */
    FLOPPY_CMD_SEEK                 = 0x0f, /* seek both heads to cylinder X */
    FLOPPY_CMD_VERSION              = 0x10, /* used during initialization, once */
    FLOPPY_CMD_SCAN_EQUAL           = 0x11, /* */
    FLOPPY_CMD_PERPENDICULAR_MODE   = 0x12, /* used during initialization, once, maybe */
    FLOPPY_CMD_CONFIGURE            = 0x13, /* set controller parameters */
    FLOPPY_CMD_LOCK                 = 0x14, /* protect controller params from a reset */
    FLOPPY_CMD_VERIFY               = 0x16, /* */
    FLOPPY_CMD_PARTID               = 24,   /* part id, extended version */
    FLOPPY_CMD_SCAN_LOW_OR_EQUAL    = 25,   /* */
    FLOPPY_CMD_SCAN_HIGH_OR_EQUAL   = 29,   /* */
};

enum fdc_version {
    FDC_NONE                        = 0x00,
    FDC_UNKNOWN                     = 0x10,
    FDC_8272A                       = 0x20, /* Intel 8272a, NEC 765 */
    FDC_765ED                       = 0x30, /* Non-Intel 1MB-compatible FDC, can't detect */
    FDC_82072                       = 0x40, /* Intel 82072; 8272a + FIFO + DUMPREGS */
    FDC_82072A                      = 0x45, /* 82072A (on Sparcs) */
    FDC_82077_ORIG                  = 0x51, /* Original version of 82077AA, sans LOCK */
    FDC_82077                       = 0x52, /* 82077AA-1 */
    FDC_82078_UNKN                  = 0x5f, /* Unknown 82078 variant */
    FDC_82078                       = 0x60, /* 44pin 82078 or 64pin 82078SL */
    FDC_82078_1                     = 0x61, /* 82078-1 (2Mbps fdc) */
    FDC_S82078B                     = 0x62, /* S82078B (first seen on Adaptec AVA-2825 VLB SCSI/EIDE/Floppy controller) */
    FDC_87306                       = 0x63, /* National Semiconductor PC 87306 */
};

/****************************************************************************************/
/*      Mnemonic                value             meaning/usage                         */

#define FLOPPY_SRA_INTP         BIT(7)
#define FLOPPY_SRA_DRV2         BIT(6)
#define FLOPPY_SRA_STEP         BIT(5)
#define FLOPPY_SRA_TRK0         BIT(4)
#define FLOPPY_SRA_HDSEL        BIT(3)
#define FLOPPY_SRA_INDX         BIT(2)
#define FLOPPY_SRA_WP           BIT(1)
#define FLOPPY_SRA_DIR          BIT(0)

#define FLOPPY_SRB_DRISEL0      BIT(5)
#define FLOPPY_SRB_WRTOGGLE     BIT(4)
#define FLOPPY_SRB_RDTOGGLE     BIT(3)
#define FLOPPY_SRB_WE           BIT(2)
#define FLOPPY_SRB_MOT1         BIT(1)
#define FLOPPY_SRB_MOT0         BIT(0)

/*
 * The Digital Output Register contains the drive select
 * and motor enable bits, a reset bit and a DMA GATE bit.
 */

#define FLOPPY_DOR_MOTOR_MASK   BIT_RANGE(7, 4) /* Drive Motor Sel Mask */
#define FLOPPY_DOR_MOTOR_D      BIT(7)          /* Set to turn drive 3's motor ON */
#define FLOPPY_DOR_MOTOR_C      BIT(6)          /* Set to turn drive 2's motor ON */
#define FLOPPY_DOR_MOTOR_B      BIT(5)          /* Set to turn drive 1's motor ON */
#define FLOPPY_DOR_MOTOR_A      BIT(4)          /* Set to turn drive 0's motor ON */
#define FLOPPY_DOR_IRQ          BIT(3)          /* Set to enable IRQs and DMA */
#define FLOPPY_DOR_RESET        BIT(2)          /* Clear = enter reset mode, Set = normal operation */
#define FLOPPY_DOR_DSEL_MASK    BIT_RANGE(1, 0) /* Select drive number for next access */
#define FLOPPY_DOR_DSEL_D       BIT_SHIFT(0, 3) /* Drive selected: 3 */
#define FLOPPY_DOR_DSEL_C       BIT_SHIFT(0, 2) /* Drive selected: 2 */
#define FLOPPY_DOR_DSEL_B       BIT_SHIFT(0, 1) /* Drive selected: 1 */
#define FLOPPY_DOR_DSEL_A       BIT_SHIFT(0, 0) /* Drive selected: 0 */

/*
 * This register allows the user to assign tape support
 * to a particular drive during initialization. Any future
 * references to that drive number automatically in-
 * vokes tape support. This register is cleared by Hard-
 * ware reset, Software resets have no effect.
 */

#define FLOPPY_TDR_SEL_MASK     BIT_RANGE(1, 0) /* Drive selected Mask */
#define FLOPPY_TDR_SEL_D        BIT_SHIFT(0, 3) /* Drive selected: 3 */
#define FLOPPY_TDR_SEL_C        BIT_SHIFT(0, 2) /* Drive selected: 2 */
#define FLOPPY_TDR_SEL_B        BIT_SHIFT(0, 1) /* Drive selected: 1 */
#define FLOPPY_TDR_SEL_A        BIT_SHIFT(0, 0) /* Drive selected: 0 */

/*
 * The Main Status Register is a read-only register and
 * is used for controlling command input and result out-
 * put for all commands.
 */

#define FLOPPY_MSR_RQM          BIT(7)          /* Set if it's OK (or mandatory) to exchange bytes with the FIFO IO port */
#define FLOPPY_MSR_DIO          BIT(6)          /* Set if FIFO IO port expects an IN opcode */
#define FLOPPY_MSR_NDMA         BIT(5)          /* Set in Execution phase of PIO mode read/write commands only */
#define FLOPPY_MSR_CB           BIT(4)          /* Command Busy: set when command byte received, cleared at end of Result phase */
#define FLOPPY_MSR_ACTD         BIT(3)          /* Drive 3 is seeking */
#define FLOPPY_MSR_ACTC         BIT(2)          /* Drive 2 is seeking */
#define FLOPPY_MSR_ACTB         BIT(1)          /* Drive 1 is seeking */
#define FLOPPY_MSR_ACTA         BIT(0)          /* Drive 0 is seeking */

/*
 * This register is included for compatibility with the
 * 82072 floppy controller and is write-only. Changing
 * the data rate changes the timings of the drive con-
 * trol signals. To ensure that drive timings are not vio-
 * lated when changing data rates, choose a drive tim-
 * ing such that the fastest data rate will not violate the
 * timing.
 */

#define FLOPPY_DSR_SWRST        BIT(7)          /* Same as DOR Reset */
#define FLOPPY_DSR_PD           BIT(6)          /* Power Down deactivates the internal clocks and shuts off the oscillator */
#define FLOPPY_DSR_COMP_MASK    BIT_RANGE(4, 2) /* Precompensation Delay Mask */
#define FLOPPY_DSR_COMP_DIS     BIT_SHIFT(2, 7) /* Precompensation Delay: Disabled */
#define FLOPPY_DSR_COMP_250     BIT_SHIFT(2, 6) /* Precompensation Delay: 250.00 ns */
#define FLOPPY_DSR_COMP_208     BIT_SHIFT(2, 5) /* Precompensation Delay: 208.33 ns */
#define FLOPPY_DSR_COMP_166     BIT_SHIFT(2, 4) /* Precompensation Delay: 166.67 ns */
#define FLOPPY_DSR_COMP_125     BIT_SHIFT(2, 3) /* Precompensation Delay: 125.00 ns */
#define FLOPPY_DSR_COMP_83      BIT_SHIFT(2, 2) /* Precompensation Delay: 83.34 ns */
#define FLOPPY_DSR_COMP_41      BIT_SHIFT(2, 1) /* Precompensation Delay: 41.67 ns */
#define FLOPPY_DSR_COMP_DEF     BIT_SHIFT(2, 0) /* Precompensation Delay: Default */
#define FLOPPY_DSR_RATE_MASK    BIT_RANGE(1, 0) /* Data Rates Mask */
#define FLOPPY_DSR_RATE_1M      BIT_SHIFT(0, 3) /* Data Rates: 1 Mbps */
#define FLOPPY_DSR_RATE_250K    BIT_SHIFT(0, 2) /* Data Rates: 250 Kbps */
#define FLOPPY_DSR_RATE_300K    BIT_SHIFT(0, 1) /* Data Rates: 300 Kbps */
#define FLOPPY_DSR_RATE_500K    BIT_SHIFT(0, 0) /* Data Rates: 500 Kbps */

#define FLOPPY_DIR_DSKCHG       BIT(7)          /*  */
#define FLOPPY_DIR_RATE_MASK    BIT_RANGE(1, 0) /* Data Rates Mask */
#define FLOPPY_DIR_RATE_1M      BIT_SHIFT(0, 3) /* Data Rates: 1 Mbps */
#define FLOPPY_DIR_RATE_250K    BIT_SHIFT(0, 2) /* Data Rates: 250 Kbps */
#define FLOPPY_DIR_RATE_300K    BIT_SHIFT(0, 1) /* Data Rates: 300 Kbps */
#define FLOPPY_DIR_RATE_500K    BIT_SHIFT(0, 0) /* Data Rates: 500 Kbps */

#define FLOPPY_CCR_RATE_MASK    BIT_RANGE(1, 0) /* Data Rates Mask */
#define FLOPPY_CCR_RATE_1M      BIT_SHIFT(0, 3) /* Data Rates: 1 Mbps */
#define FLOPPY_CCR_RATE_250K    BIT_SHIFT(0, 2) /* Data Rates: 250 Kbps */
#define FLOPPY_CCR_RATE_300K    BIT_SHIFT(0, 1) /* Data Rates: 300 Kbps */
#define FLOPPY_CCR_RATE_500K    BIT_SHIFT(0, 0) /* Data Rates: 500 Kbps */

#define FLOPPY_ST0_INTR         BIT_RANGE(7, 6) /* Interrupt code mask */
#define FLOPPY_ST0_RDYCHG       BIT_SHIFT(6, 3) /* */
#define FLOPPY_ST0_INVCMD       BIT_SHIFT(6, 2) /* */
#define FLOPPY_ST0_INTR0        BIT_SHIFT(6, 1) /* */
#define FLOPPY_ST0_SE           BIT(5)          /* Seek end */
#define FLOPPY_ST0_ECE          BIT(4)          /* Equipment check error */
#define FLOPPY_ST0_NR           BIT(3)          /* Not Ready */
#define FLOPPY_ST0_HA           BIT(2)          /* Head (Address) */
#define FLOPPY_ST0_DS           BIT_RANGE(1, 0) /* drive select mask */
#define FLOPPY_ST0_DS_D         BIT_SHIFT(0, 3) /* drive 3 select */
#define FLOPPY_ST0_DS_C         BIT_SHIFT(0, 2) /* drive 2 select */
#define FLOPPY_ST0_DS_B         BIT_SHIFT(0, 1) /* drive 1 select */
#define FLOPPY_ST0_DS_A         BIT_SHIFT(0, 0) /* drive 0 select */

#define FLOPPY_ST1_EOC          BIT(7)          /* End Of Cylinder */
#define FLOPPY_ST1_CRC          BIT(5)          /* CRC error in data or addr */
#define FLOPPY_ST1_OR           BIT(4)          /* OverRun */
#define FLOPPY_ST1_ND           BIT(2)          /* No Data - unreadable */
#define FLOPPY_ST1_WP           BIT(1)          /* Write Protect */
#define FLOPPY_ST1_MAM          BIT(0)          /* Missing Address Mark */

#define FLOPPY_ST2_CM           BIT(6)          /* Control Mark = deleted */
#define FLOPPY_ST2_CRC          BIT(5)          /* CRC error in data field */
#define FLOPPY_ST2_WC           BIT(4)          /* Wrong Cylinder */
#define FLOPPY_ST2_SEH          BIT(3)          /* Scan Equal Hit */
#define FLOPPY_ST2_SNS          BIT(2)          /* Scan Not Satisfied */
#define FLOPPY_ST2_BC           BIT(1)          /* Cylinder Marked Bad */
#define FLOPPY_ST2_MAM          BIT(0)          /* Missing Address Mark (again) */

#define FLOPPY_SPECIFY1_SRT     BIT_RANGE(7, 4)
#define FLOPPY_SPECIFY1_HUT     BIT_RANGE(3, 0)
#define FLOPPY_SPECIFY2_HLT     BIT_RANGE(7, 1)
#define FLOPPY_SPECIFY2_NODMA   BIT(0)

#define FLOPPY_SECTOR_SIZE_16K  0x07
#define FLOPPY_SECTOR_SIZE_8K   0x06
#define FLOPPY_SECTOR_SIZE_4K   0x05
#define FLOPPY_SECTOR_SIZE_2K   0x04
#define FLOPPY_SECTOR_SIZE_1K   0x03
#define FLOPPY_SECTOR_SIZE_512  0x02
#define FLOPPY_SECTOR_SIZE_256  0x01
#define FLOPPY_SECTOR_SIZE_128  0x00

#endif /* _DRIVER_FLOPPY_FLOPPY_H_ */
