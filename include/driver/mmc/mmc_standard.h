/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MMC_STANDARD_H_
#define _DRIVER_MMC_STANDARD_H_

#include <types.h>

enum mmc_command {
    /* Basic commands and read-stream command */
    MMC_GO_IDLE_STATE               = 0,
    MMC_SEND_OP_COND                = 1,
    MMC_ALL_SEND_CID                = 2,
    MMC_SET_RELATIVE_ADDR           = 3,
    MMC_SET_DSR                     = 4,
    MMC_SLEEP_AWAKE                 = 5,
    MMC_SWITCH                      = 6,
    MMC_SELECT_CARD                 = 7,
    MMC_SEND_EXT_CSD                = 8,
    MMC_SEND_CSD                    = 9,
    MMC_SEND_CID                    = 10,
    MMC_READ_DAT_UNTIL_STOP         = 11,
    MMC_STOP_TRANSMISSION           = 12,
    MMC_SEND_STATUS                 = 13,
    MMC_BUS_TEST_R                  = 14,
    MMC_GO_INACTIVE_STATE           = 15,
    MMC_BUS_TEST_W                  = 19,

    /* Block-oriented read commands */
    MMC_SET_BLOCKLEN                = 16,
    MMC_READ_SINGLE_BLOCK           = 17,
    MMC_READ_MULTIPLE_BLOCK         = 18,

    /* Stream write commands */
    MMC_MMC_WRITE_DAT_UNTIL_STOP    = 20,

    /* Block-oriented write commands */
    MMC_SET_BLOCK_COUNT             = 23,
    MMC_WRITE_BLOCK                 = 24,
    MMC_WRITE_MULTIPLE_BLOCK        = 25,
    MMC_PROGRAM_CID                 = 26,
    MMC_PROGRAM_CSD                 = 27,

    /* Block-oriented write protection commands */
    MMC_SET_WRITE_PROT              = 28,
    MMC_CLR_WRITE_PROT              = 29,
    MMC_SEND_WRITE_PROT             = 30,
    MMC_SEND_WRITE_PROT_TYPE        = 31,

    /* Erase commands */
    MMC_ERASE_GROUP_START           = 35,
    MMC_ERASE_GROUP_END             = 36,
    MMC_ERASE                       = 38,

    /* I/O mode commands */
    MMC_FAST_IO                     = 39,
    MMC_GO_IRQ_STATE                = 40,

    /* Lock card commands */
    MMC_LOCK_UNLOCK                 = 42,

    /* Application-specific commands */
    MMC_APP_CMD                     = 55,
    MMC_GEN_CMD                     = 56,
};

#define MMC_OCR_BUSY        0x80000000
#define MMC_OCR_ACCESS_MOD  0x60000000
#define MMC_OCR_HCS         0x40000000
#define MMC_OCR_S18R        0x01000000
#define MMC_OCR_VOLTA       0x007fff80

#define MMC_VOLTA_16_19     0x00000080
#define MMC_VOLTA_20_21     0x00000100
#define MMC_VOLTA_21_22     0x00000200
#define MMC_VOLTA_22_23     0x00000400
#define MMC_VOLTA_23_24     0x00000800
#define MMC_VOLTA_24_25     0x00001000
#define MMC_VOLTA_25_26     0x00002000
#define MMC_VOLTA_26_27     0x00004000
#define MMC_VOLTA_27_28     0x00008000
#define MMC_VOLTA_28_29     0x00010000
#define MMC_VOLTA_29_30     0x00020000
#define MMC_VOLTA_30_31     0x00040000
#define MMC_VOLTA_31_32     0x00080000
#define MMC_VOLTA_32_33     0x00100000
#define MMC_VOLTA_33_34     0x00200000
#define MMC_VOLTA_34_35     0x00400000
#define MMC_VOLTA_35_36     0x00800000

struct mmc_cid {
    uint32_t    manfid;
    uint8_t     prod_name[8];
    uint8_t     prv;
    uint32_t    serial;
    uint16_t    oemid;
    uint16_t    year;
    uint8_t     hwrev;
    uint8_t     fwrev;
    uint8_t     month;
};

#endif /* _DRIVER_MMC_STANDARD_H_ */
