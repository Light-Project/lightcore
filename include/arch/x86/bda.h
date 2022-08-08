/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_BDA_H_
#define _ARCH_X86_BDA_H_

#ifndef X86_BDA_OFFSET
# define X86_BDA_OFFSET 0x400
#endif

enum bios_data_area {
/*  Mnemonic            Offset Hex                  BIOS Service    Field Size  Function    */
    X86_BDA_COM1        = X86_BDA_OFFSET + 0x00,    /* Int 14h      2 bytes     Base I/O address for serial port 1 (communications port 1 - COM 1) */
    X86_BDA_COM2        = X86_BDA_OFFSET + 0x02,    /* Int 14h      2 bytes     Base I/O address for serial port 2 (communications port 2 - COM 2) */
    X86_BDA_COM3        = X86_BDA_OFFSET + 0x04,    /* Int 14h      2 bytes     Base I/O address for serial port 3 (communications port 3 - COM 3) */
    X86_BDA_COM4        = X86_BDA_OFFSET + 0x06,    /* Int 14h      2 bytes     Base I/O address for serial port 4 (communications port 4 - COM 4) */
    X86_BDA_LPT1        = X86_BDA_OFFSET + 0x08,    /* Int 17h      2 bytes     Base I/O address for parallel port 1 (printer port 1 - LPT 1) */
    X86_BDA_LPT2        = X86_BDA_OFFSET + 0x0a,    /* Int 17h      2 bytes     Base I/O address for parallel port 2 (printer port 2 - LPT 2) */
    X86_BDA_LPT3        = X86_BDA_OFFSET + 0x0c,    /* Int 17h      2 bytes     Base I/O address for parallel port 2 (printer port 3 - LPT 3) */
    X86_BDA_LPT4        = X86_BDA_OFFSET + 0x0e,    /* Int 17h      2 bytes     Base I/O address for parallel port 4 (printer port 4 - LPT 4) (Only found in PC/XT systems) */
    X86_BDA_EQUIPMENT   = X86_BDA_OFFSET + 0x10,    /* Int 11h      2 bytes     Equipment Word */
    X86_BDA_INTERRUPT   = X86_BDA_OFFSET + 0x12,    /* POST         1 byte      Interrupt flag - Manufacturing test */
    X86_BDA_MEMORY      = X86_BDA_OFFSET + 0x13,    /* Int 12h      2 bytes     Memory size in Kb */
    X86_BDA_ATERROR     = X86_BDA_OFFSET + 0x15,    /*              2 bytes     Error codes for AT+; Adapter memory size for PC and XT */
    X86_BDA_KBDFLAG1    = X86_BDA_OFFSET + 0x17,    /* Int 16h      1 byte      Keyboard shift flags 1 */
    X86_BDA_KBDFLAG2    = X86_BDA_OFFSET + 0x18,    /* Int 16h      1 byte      Keyboard shift flags 2 */
    X86_BDA_ALTNPAD     = X86_BDA_OFFSET + 0x19,    /* Int 09h      1 byte      Alt Numpad work area */
    X86_BDA_KBDBUFNC    = X86_BDA_OFFSET + 0x1a,    /* Int 16h      2 bytes     Pointer to the address of the next character in the keyboard buffer */
    X86_BDA_KBDBUFLC    = X86_BDA_OFFSET + 0x1c,    /* Int 16h      2 bytes     Pointer to the address of the last character in he keyboard buffer */
    X86_BDA_KBDBUFF     = X86_BDA_OFFSET + 0x1e,    /* Int 16h      32 bytes    Keyboard buffer */
    X86_BDA_FLOPPYCA    = X86_BDA_OFFSET + 0x3e,    /* Int 13h      1 byte      Floppy disk drive calibration status */
    X86_BDA_FLOPPYMS    = X86_BDA_OFFSET + 0x3f,    /* Int 13h      1 byte      Floppy disk drive motor status */
    X86_BDA_FLOPPYMT    = X86_BDA_OFFSET + 0x40,    /* Int 13h      1 byte      Floppy disk drive motor time-out */
    X86_BDA_FLOPPYDS    = X86_BDA_OFFSET + 0x41,    /* Int 13h      1 byte      Floppy disk drive status */
    X86_BDA_DISKSTA0    = X86_BDA_OFFSET + 0x42,    /* Int 13h      1 byte      Hard disk and floppy controller status register 0 */
    X86_BDA_FLOPPYS1    = X86_BDA_OFFSET + 0x43,    /* Int 13h      1 byte      Floppy drive controller status register 1 */
    X86_BDA_FLOPPYS2    = X86_BDA_OFFSET + 0x44,    /* Int 13h      1 byte      Floppy drive controller status register 2 */
    X86_BDA_FLOPPYCN    = X86_BDA_OFFSET + 0x45,    /* Int 13h      1 byte      Floppy disk controller: cylinder number */
    X86_BDA_FLOPPYHN    = X86_BDA_OFFSET + 0x46,    /* Int 13h      1 byte      Floppy disk controller: head number */
    X86_BDA_FLOPPYSN    = X86_BDA_OFFSET + 0x47,    /* Int 13h      1 byte      Floppy disk controller: sector number */
    X86_BDA_FLOPPYWB    = X86_BDA_OFFSET + 0x48,    /*              1 byte      Floppy disk controller: number of byte written */
    X86_BDA_VGAACTMOD   = X86_BDA_OFFSET + 0x49,    /* Int 10h      1 byte      Active video mode setting */
    X86_BDA_VGATXTCOL   = X86_BDA_OFFSET + 0x4a,    /* Int 10h      2 bytes     Number of textcolumns per row for the active video mode */
    X86_BDA_VGAACTBYTE  = X86_BDA_OFFSET + 0x4c,    /* Int 10h      2 bytes     Size of active video in page bytes */
    X86_BDA_VGAACTREL   = X86_BDA_OFFSET + 0x4e,    /* Int 10h      2 bytes     Offset address of the active video page relative to the start of video RAM */
    X86_BDA_VGACP0      = X86_BDA_OFFSET + 0x50,    /* Int 10h      2 bytes     Cursor position for video page 0 */
    X86_BDA_VGACP1      = X86_BDA_OFFSET + 0x52,    /* Int 10h      2 bytes     Cursor position for video page 1 */
    X86_BDA_VGACP2      = X86_BDA_OFFSET + 0x54,    /* Int 10h      2 bytes     Cursor position for video page 2 */
    X86_BDA_VGACP3      = X86_BDA_OFFSET + 0x56,    /* Int 10h      2 bytes     Cursor position for video page 3 */
    X86_BDA_VGACP4      = X86_BDA_OFFSET + 0x58,    /* Int 10h      2 bytes     Cursor position for video page 4 */
    X86_BDA_VGACP5      = X86_BDA_OFFSET + 0x5a,    /* Int 10h      2 bytes     Cursor position for video page 5 */
    X86_BDA_VGACP6      = X86_BDA_OFFSET + 0x5c,    /* Int 10h      2 bytes     Cursor position for video page 6 */
    X86_BDA_VGACP7      = X86_BDA_OFFSET + 0x5e,    /* Int 10h      2 bytes     Cursor position for video page 7 */
    X86_BDA_CURSHP      = X86_BDA_OFFSET + 0x60,    /* Int 10h      2 bytes     Cursor shape */
    X86_BDA_VGAPAGE     = X86_BDA_OFFSET + 0x62,    /* Int 10h      1 byte      Active video page */
    X86_BDA_VGAIO       = X86_BDA_OFFSET + 0x63,    /* Int 10h      2 bytes     I/O port address for the video display adapter */
    X86_BDA_APTMOD      = X86_BDA_OFFSET + 0x65,    /* Int 10h      1 byte      Video display adapter internal mode register */
    X86_BDA_COLPAL      = X86_BDA_OFFSET + 0x66,    /* Int 10h      1 byte      Color palette */
    X86_BDA_APTROMOFF   = X86_BDA_OFFSET + 0x67,    /*              2 bytes     Adapter ROM offset address */
    X86_BDA_APTROMSEG   = X86_BDA_OFFSET + 0x69,    /*              2 bytes     Adapter ROM segment address */
    X86_BDA_LASTINT     = X86_BDA_OFFSET + 0x6b,    /*              1 byte      Last interrupt (not PC) */
    X86_BDA_CNT1AH      = X86_BDA_OFFSET + 0x6c,    /* Int 1Ah      4 bytes     Counter for Interrupt 1Ah */
    X86_BDA_TIM24H      = X86_BDA_OFFSET + 0x70,    /* Int 1Ah      1 byte      Timer 24 hour flag */
    X86_BDA_KBDCTRLBF   = X86_BDA_OFFSET + 0x71,    /* Int 16h      1 byte      Keyboard Ctrl-Break flag */
    X86_BDA_SOFTRESET   = X86_BDA_OFFSET + 0x72,    /* POST         2 bytes     Soft reset flag */
    X86_BDA_DISKLSTSTA  = X86_BDA_OFFSET + 0x74,    /* Int 13h      1 byte      Status of last hard disk operation */
    X86_BDA_DISKNUM     = X86_BDA_OFFSET + 0x75,    /* Int 13h      1 byte      Number of hard disk drives */
    X86_BDA_DISKCTL     = X86_BDA_OFFSET + 0x76,    /* Int 13h      1 byte      Hard disk control byte */
    X86_BDA_DISKIO      = X86_BDA_OFFSET + 0x77,    /* Int 13h      1 byte      Offset address of hard disk I/O port (XT) */
    X86_BDA_LPT1TO      = X86_BDA_OFFSET + 0x78,    /* Int 17h      1 byte      Parallel port 1 timeout */
    X86_BDA_LPT2TO      = X86_BDA_OFFSET + 0x79,    /* Int 17h      1 byte      Parallel port 2 timeout */
    X86_BDA_LPT3TO      = X86_BDA_OFFSET + 0x7a,    /* Int 17h      1 byte      Parallel port 3 timeout */
    X86_BDA_LPT4TO      = X86_BDA_OFFSET + 0x7b,    /*              1 byte      Parallel port 4 timeout (PC, XT) support for virtual DMA services (VDS) */
    X86_BDA_COM1TO      = X86_BDA_OFFSET + 0x7c,    /* Int 14h      1 byte      Serial port 1 timeout */
    X86_BDA_COM2TO      = X86_BDA_OFFSET + 0x7d,    /* Int 14h      1 byte      Serial port 2 timeout */
    X86_BDA_COM3TO      = X86_BDA_OFFSET + 0x7e,    /* Int 14h      1 byte      Serial port 3 timeout */
    X86_BDA_COM4TO      = X86_BDA_OFFSET + 0x7f,    /* Int 14h      1 byte      Serial port 4 timeout */
    X86_BDA_KBDBUFSA    = X86_BDA_OFFSET + 0x80,    /* Int 16h      2 bytes     Starting address of keyboard buffer */
    X86_BDA_KBDBUFEA    = X86_BDA_OFFSET + 0x82,    /* Int 16h      2 bytes     Ending address of keyboard buffer */
    X86_BDA_VGAROWS     = X86_BDA_OFFSET + 0x84,    /* Int 10h      1 byte      Number of video rows (minus 1) */
    X86_BDA_SCANPCHAR   = X86_BDA_OFFSET + 0x85,    /* Int 10h      2 bytes     Number of scan lines per character */
    X86_BDA_APTOPT      = X86_BDA_OFFSET + 0x87,    /* Int 10h      1 byte      Video display adapter options */
    X86_BDA_APTSWH      = X86_BDA_OFFSET + 0x88,    /* Int 10h      1 byte      Video display adapter switches */
    X86_BDA_VGAFLG1     = X86_BDA_OFFSET + 0x89,    /* Int 10h      1 byte      VGA video flags 1 */
    X86_BDA_VGAFLG2     = X86_BDA_OFFSET + 0x8a,    /* Int 10h      1 byte      VGA video flags 2 */
    X86_BDA_FLOPPYCD    = X86_BDA_OFFSET + 0x8b,    /* Int 13h      1 byte      Floppy disk configuration data */
    X86_BDA_DISKCS      = X86_BDA_OFFSET + 0x8c,    /* Int 13h      1 byte      Hard disk drive controller status */
    X86_BDA_DISKDE      = X86_BDA_OFFSET + 0x8d,    /* Int 13h      1 byte      Hard disk drive error */
    X86_BDA_DISKTCF     = X86_BDA_OFFSET + 0x8e,    /* Int 13h      1 byte      Hard disk drive task complete flag */
    X86_BDA_DISKDI      = X86_BDA_OFFSET + 0x8f,    /* Int 13h      1 byte      Floppy disk drive information */
    X86_BDA_DISK0M      = X86_BDA_OFFSET + 0x90,    /* Int 13h      1 byte      Diskette 0 media state */
    X86_BDA_DISK1M      = X86_BDA_OFFSET + 0x91,    /* Int 13h      1 byte      Diskette 1 media state */
    X86_BDA_DISK0O      = X86_BDA_OFFSET + 0x92,    /* Int 13h      1 byte      Diskette 0 operational starting state */
    X86_BDA_DISK1O      = X86_BDA_OFFSET + 0x93,    /* Int 13h      1 byte      Diskette 1 operational starting status */
    X86_BDA_DISK0C      = X86_BDA_OFFSET + 0x94,    /* Int 13h      1 byte      Diskette 0 current cylinder */
    X86_BDA_DISK1C      = X86_BDA_OFFSET + 0x95,    /* Int 13h      1 byte      Diskette 1 current cylinder */
    X86_BDA_KBDFLAG3    = X86_BDA_OFFSET + 0x96,    /* Int 16h      1 byte      Keyboard status flags 3 */
    X86_BDA_KBDFLAG4    = X86_BDA_OFFSET + 0x97,    /* Int 16h      1 byte      Keyboard status flags 4 */
    X86_BDA_UWAITP      = X86_BDA_OFFSET + 0x98,    /*              4 bytes     Segment:Offset address of user wait flag pointer */
    X86_BDA_UWAITC      = X86_BDA_OFFSET + 0x9c,    /*              4 bytes     User wait count */
    X86_BDA_UWAITF      = X86_BDA_OFFSET + 0xa0,    /*              1 byte      User wait flag */
    X86_BDA_LAN         = X86_BDA_OFFSET + 0xa1,    /*              7 bytes     Local area network (LAN) bytes */
    X86_BDA_VGACTLBLK   = X86_BDA_OFFSET + 0xa8,    /*              4 bytes     Segment:Offset address of video parameter control block */
    X86_BDA_INTRAAREA   = X86_BDA_OFFSET + 0xf0,    /*              16 bytes    Intra-applications communications area */
};

#endif  /* _ARCH_X86_BDA_H_ */
