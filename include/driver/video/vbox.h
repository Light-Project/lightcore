/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _DRIVER_GPU_VBOX_H_
#define _DRIVER_GPU_VBOX_H_

enum vbox_registers {
    VBE_DISPI_IOPORT_INDEX              = 0x01CE,
    VBE_DISPI_IOPORT_DATA               = 0x01CF,
};

enum vbox_index {
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
    VBE_DISPI_INDEX_FB_BASE_HI          = 0xb,
};

#define VBE_DISPI_ID0                   0xB0C0
#define VBE_DISPI_ID1                   0xB0C1
#define VBE_DISPI_ID2                   0xB0C2
#define VBE_DISPI_ID3                   0xB0C3
#define VBE_DISPI_ID4                   0xB0C4
#define VBE_DISPI_ID_VBOX_VIDEO         0xBE00

#define VBE_DISPI_ID_HGSMI              0xBE01
#define VBE_DISPI_ID_ANYX               0xBE02

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_GETCAPS               0x02
#define VBE_DISPI_8BIT_DAC              0x20

#define VGA_PORT_HGSMI_HOST             0x3b0
#define VGA_PORT_HGSMI_GUEST            0x3d0

#endif  /* _DRIVER_GPU_VBOX_H_ */
