/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_VIDEO_BOCHS_H_
#define _DRIVER_VIDEO_BOCHS_H_

enum bochs_registers {
    VBE_DISPI_IOPORT_INDEX              = 0x01ce,
    VBE_DISPI_IOPORT_DATA               = 0x01cf,
};

enum bochs_index {
    VBE_DISPI_INDEX_ID                  = 0x0,
    VBE_DISPI_INDEX_XRES                = 0x1,
    VBE_DISPI_INDEX_YRES                = 0x2,
    VBE_DISPI_INDEX_BPP                 = 0x3,
    VBE_DISPI_INDEX_ENABLE              = 0x4,
    VBE_DISPI_INDEX_BANK                = 0x5,
    VBE_DISPI_INDEX_VIRT_WIDTH          = 0x6,
    VBE_DISPI_INDEX_VIRT_HEIGHT         = 0x7,
    VBE_DISPI_INDEX_X_OFFSET            = 0x8,
    VBE_DISPI_INDEX_Y_OFFSET            = 0x9,
    VBE_DISPI_INDEX_VIDEO_MEMORY_64K    = 0xa,
};

#define VBE_DISPI_ID0                    0xb0c0
#define VBE_DISPI_ID1                    0xb0c1
#define VBE_DISPI_ID2                    0xb0c2
#define VBE_DISPI_ID3                    0xb0c3
#define VBE_DISPI_ID4                    0xb0c4
#define VBE_DISPI_ID5                    0xb0c5

#define VBE_DISPI_DISABLED               0x00
#define VBE_DISPI_ENABLED                0x01
#define VBE_DISPI_GETCAPS                0x02
#define VBE_DISPI_8BIT_DAC               0x20
#define VBE_DISPI_LFB_ENABLED            0x40
#define VBE_DISPI_NOCLEARMEM             0x80

#endif /* _DRIVER_VIDEO_BOCHS_H_ */
