/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/floppy.h
 *  x86 floppy register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-14      first version 
 * 
 */

#ifndef _CHIP_X86_IDE_
#define _CHIP_X86_IDE_

#define IDE0_CMD_BASE   ((uint16_t)0x01F0)
#define IDE1_CMD_BASE   ((uint16_t)0x0170)

#define IDE0_CTL_BASE   ((uint16_t)0x03f4)
#define IDE1_CTL_BASE   ((uint16_t)0x0370)

enum IDE_Command_Block
{
    IDE0_DATA_REGISTER      = IDE0_CMD_BASE + 0,
    IDE0_ERROR_REGISTER     = IDE0_CMD_BASE + 1,
    IDE0_FEATURE_REGISTER   = IDE0_CMD_BASE + 1,
    IDE0_CNT_REGISTER       = IDE0_CMD_BASE + 2,
    IDE0_SEC_REGISTER       = IDE0_CMD_BASE + 3,
    IDE0_CYLL_REGISTER      = IDE0_CMD_BASE + 4,
    IDE0_CYLH_REGISTER      = IDE0_CMD_BASE + 5,
    IDE0_DRIVER_REGISTER    = IDE0_CMD_BASE + 6,
    IDE0_CMD_REGISTER       = IDE0_CMD_BASE + 7,
    IDE0_STATUS_REGISTER    = IDE0_CMD_BASE + 7,

    IDE1_DATA_REGISTER      = IDE1_CMD_BASE + 0,
    IDE1_ERROR_REGISTER     = IDE1_CMD_BASE + 1,
    IDE1_FEATURE_REGISTER   = IDE1_CMD_BASE + 1,
    IDE1_CNT_REGISTER       = IDE1_CMD_BASE + 2,
    IDE1_SEC_REGISTER       = IDE1_CMD_BASE + 3,
    IDE1_CYLL_REGISTER      = IDE1_CMD_BASE + 4,
    IDE1_CYLH_REGISTER      = IDE1_CMD_BASE + 5,
    IDE1_DRIVER_REGISTER    = IDE1_CMD_BASE + 6,
    IDE1_CMD_REGISTER       = IDE1_CMD_BASE + 7,
    IDE1_STATUS_REGISTER    = IDE1_CMD_BASE + 7,
};

enum IDE_Control_Block
{
    IDE0_CTL_REGISTER   = IDE0_CTL_BASE + 2,
    IDE0_ISA_REGISTER   = IDE0_CTL_BASE + 3,

    IDE1_CTL_REGISTER   = IDE1_CTL_BASE + 2,
    IDE1_ISA_REGISTER   = IDE1_CTL_BASE + 3,
};

enum IDE_Registers
{
    IDE_BMICP   = 0x00,    // BMICP: Bus Master IDE Command Primary
    IDE_BMISP   = 0x02,    // BMISP: Bus Master IDE Status Primary
    IDE_BMIDP   = 0x04,    // BMIDP: Bus Master IDE Descriptor Table Pointer Primary
};

enum IDE_Command
{
    IDE_COMMAND_NOP                                     = 0x00,
    IDE_COMMAND_CFA_RESERVED0                           = 0X03,
    IDE_COMMAND_DATA_SET_MANAGEMENT                     = 0x06,
    IDE_COMMAND_DEVICE_RESET                            = 0x08,
    IDE_COMMAND_REQUEST_SENSE                           = 0x0B,
    IDE_COMMAND_RECALIBRATE                             = 0x10,
    IDE_COMMAND_READ_SECTORS_WITH_RETRY                 = 0x20,
    IDE_COMMAND_READ_SECTORS_WITHOUT_RETRY              = 0x21,
    IDE_COMMAND_READ_LONG_WITH_RETRY                    = 0x22,
    IDE_COMMAND_READ_LONG_WITHOUT_RETRY                 = 0x23,
    IDE_COMMAND_READ_SECTORS_EXT                        = 0x24,
    IDE_COMMAND_READ_DMA_EXT                            = 0x25,
    IDE_COMMAND_READ_DMA_QUEUED_EXT                     = 0x26,
    IDE_COMMAND_READ_NATIVE_MAX_ADDRESS_EXT             = 0x27,
    IDE_COMMAND_READ_MULTIPLE_EXT                       = 0x29,
    IDE_COMMAND_READ_STREAM_DMA                         = 0x2A,
    IDE_COMMAND_READ_STREAM                             = 0x2B,
    IDE_COMMAND_READ_LOG_EXT                            = 0x2F,
    IDE_COMMAND_WRITE_SECTORS_WITH_RETRY                = 0x30,
    IDE_COMMAND_WRITE_SECTORS_WITHOUT_RETRY             = 0x31,
    IDE_COMMAND_WRITE_LONG_WITH_RETRY                   = 0x32,
    IDE_COMMAND_WRITE_LONG_WITHOUT_RETRY                = 0x33,
    IDE_COMMAND_WRITE_SECTORS_EXT                       = 0x34,
    IDE_COMMAND_WRITE_DMA_EXT                           = 0x35,
    IDE_COMMAND_WRITE_DMA_QUEUED_EXT                    = 0x36,
    IDE_COMMAND_SET_NATIVE_MAX_ADDRESS_EXT              = 0x37,
    IDE_COMMAND_CFA_RESERVED1                           = 0x38,
    IDE_COMMAND_WRITE_MULTIPLE_EXT                      = 0x39,
    IDE_COMMAND_WRITE_STREAM_DMA                        = 0x3A,
    IDE_COMMAND_WRITE_STREAM                            = 0x3B,
    IDE_COMMAND_WRITE_VERIFY                            = 0x3C,
    IDE_COMMAND_WRITE_DMA_FUA_EXT                       = 0x3D,
    IDE_COMMAND_WRITE_DMA_QUEUED_FUA_EXT                = 0x3E,
    IDE_COMMAND_WRITE_LOG_EXT                           = 0x3F,
    IDE_COMMAND_READ_VERIFY_SECTORS_WITH_RETRY          = 0x40,
    IDE_COMMAND_READ_VERIFY_SECTORS_WITHOUT_RETRY       = 0x41,
    IDE_COMMAND_READ_VERIFY_SECTORS_EXT                 = 0x42,
    IDE_COMMAND_Trusted_Non_Data                        = 0x5B,
    IDE_COMMAND_Trusted_Receive                         = 0x5C,
    IDE_COMMAND_Trusted_Receive_DMA                     = 0x5D,
    IDE_COMMAND_Trusted_Send                            = 0x5E,
    IDE_COMMAND_Trusted_Send_DMA                        = 0x5F,
    IDE_COMMAND_Read_FPDMA_Queued                       = 0x60,
    IDE_COMMAND_Write_FPDMA_Queued                      = 0x61,
    IDE_COMMAND_SATA_Reserved0                          = 0x62,
    IDE_COMMAND_NCQ_Queue_Management                    = 0x63,
    IDE_COMMAND_Send_FPDMA_Queued                       = 0x64,
    IDE_COMMAND_Receive_FPDMA_Queued                    = 0x65,
    IDE_COMMAND_SATA_Reserved1                          = 0x66,
    IDE_COMMAND_SATA_Reserved2                          = 0x67,
    IDE_COMMAND_SEEK                                    = 0x70,
    IDE_COMMAND_SET_DATETIME_EXT                        = 0x77,
    IDE_COMMAND_ACCESSIBLE_MAX_ADDRESS_CONFIGURATION    = 0x78,
    IDE_COMMAND_SMART                                   = 0xB0,
    IDE_COMMAND_READ_DMA                                = 0xC8,
    IDE_COMMAND_WRITE_DMA                               = 0xCA,
    IDE_COMMAND_CACHE_FLUSH                             = 0xE7,
    IDE_COMMAND_CACHE_FLUSH_EXT                         = 0xEA,
    IDE_COMMAND_PACKET                                  = 0xA0,
    IDE_COMMAND_IDENTIFY_PACKET                         = 0xA1,
    IDE_COMMAND_IDENTIFY                                = 0xEC,
    IDE_COMMAND_GET_MEDIA_STATUS                        = 0xDA,
    IDE_COMMAND_ACKNOWLEDGE_MEDIA_CHANGE                = 0xDB,
    IDE_COMMAND_BOOT_POST_BOOT                          = 0xDC,
    IDE_COMMAND_BOOT_PRE_BOOT                           = 0xDD,
    IDE_COMMAND_MEDIA_LOCK                              = 0xDE,
    IDE_COMMAND_MEDIA_UNLOCK                            = 0xDF,
    IDE_COMMAND_STANDBY_IMMEDIATE                       = 0xE0,
    IDE_COMMAND_IDLE_IMMEDIATE                          = 0xE1,
    IDE_COMMAND_STANDBY                                 = 0xE2,
    IDE_COMMAND_IDLE                                    = 0xE3,
    IDE_COMMAND_READ_BUFFER                             = 0xE4,
    IDE_COMMAND_CHECK_POWER_MODE                        = 0xE5,
    IDE_COMMAND_SLEEP                                   = 0xE6,
    IDE_COMMAND_FLUSH_CACHE                             = 0xE7,
    IDE_COMMAND_WRITE_BUFFER                            = 0xE8,
    IDE_COMMAND_READ_BUFFER_DMA                         = 0xE9,
    IDE_COMMAND_FLUSH_CACHE_EXT                         = 0xEA,
    IDE_COMMAND_WRITE_BUFFER_DMA                        = 0xEB,
    IDE_COMMAND_IDENTIFY_DEVICE                         = 0xEC,
    IDE_COMMAND_MEDIA_EJECT                             = 0xED,
    IDE_COMMAND_IDENTIFY_DEVICE_DMA                     = 0xEE,
    IDE_COMMAND_SET_FEATURES                            = 0xEF,
    IDE_COMMAND_VENDOR_SPECIFIC                         = 0xF0,
    IDE_COMMAND_SECURITY_SET_PASSWORD                   = 0xF1,
    IDE_COMMAND_SECURITY_UNLOCK                         = 0xF2,
    IDE_COMMAND_SECURITY_ERASE_PREPARE                  = 0xF3,
    IDE_COMMAND_SECURITY_ERASE_UNIT                     = 0xF4,
    IDE_COMMAND_SECURITY_FREEZE_LOCK                    = 0xF5,
    IDE_COMMAND_SECURITY_DISABLE_PASSWORD               = 0xF6,
    IDE_COMMAND_READ_NATIVE_MAX_ADDRESS                 = 0xF8,
    IDE_COMMAND_SET_MAX_ADDRESS                         = 0xF9,
    IDE_COMMAND_VENDOR0                                 = 0xFA,
    IDE_COMMAND_VENDOR1                                 = 0xFB,
    IDE_COMMAND_VENDOR2                                 = 0xFC,
    IDE_COMMAND_VENDOR3                                 = 0xFD,
    IDE_COMMAND_VENDOR4                                 = 0xFE,
    IDE_COMMAND_VENDOR5                                 = 0xFF,
};

/*************************************************************************************/
/*      Mnemonic                 value        meaning/usage                          */

/* STATUS bitflag definitions */
#define IDE_STATUS_BSY          0x80        // Busy
#define IDE_STATUS_DRDY         0x40        // Drive ready
#define IDE_STATUS_DF           0x20        // Drive write fault
#define IDE_STATUS_DSC          0x10        // Drive seek complete
#define IDE_STATUS_DRQ          0x08        // Data request ready
#define IDE_STATUS_CORR         0x04        // Corrected data
#define IDE_STATUS_IDX          0x02        // Index
#define IDE_STATUS_ERR          0x01        // Error

/* ERROR bitflag definitions */
#define IDE_ERROR_BBK           0x80        // Bad block
#define IDE_ERROR_UNC           0x40        // Uncorrectable data
#define IDE_ERROR_MC            0x20        // Media changed
#define IDE_ERROR_IDNF          0x10        // ID mark not found
#define IDE_ERROR_MCR           0x08        // Media change request
#define IDE_ERROR_ABRT          0x04        // Command aborted
#define IDE_ERROR_TK0NF         0x02        // Track 0 not found
#define IDE_ERROR_AMNF          0x01        // No address mark

#endif
