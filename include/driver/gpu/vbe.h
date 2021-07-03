/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/chip/x86/vbe.h
 *  x86 vbe register map
 *  (C) 2020 wzl finishing
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-03-14      first version 
 * 
 */


#ifndef _DRIVER_GPU_VBE_H_
#define _DRIVER_GPU_VBE_H_

#include <types.h>

struct vbe_info {
    uint32_t signature;
    uint16_t version;
    struct segoff_s oem_string;
    uint32_t capabilities;
    struct segoff_s video_mode;
    uint16_t total_memory;
    uint16_t oem_revision;
    struct segoff_s oem_vendor_string;
    struct segoff_s oem_product_string;
    struct segoff_s oem_revision_string;
    uint8_t reserved[222];
} __packed;

struct vbe_mode_info_block{
  /* Mandatory information for all VBE revisions.  */
  uint16_t mode_attributes;
  uint8_t win_a_attributes;
  uint8_t win_b_attributes;
  uint16_t win_granularity;
  uint16_t win_size;
  uint16_t win_a_segment;
  uint16_t win_b_segment;
  uint32_t win_func_ptr;
  uint16_t bytes_per_scan_line;

  /* Mandatory information for VBE 1.2 and above.  */
  uint16_t x_resolution;
  uint16_t y_resolution;
  uint8_t x_char_size;
  uint8_t y_char_size;
  uint8_t number_of_planes;
  uint8_t bits_per_pixel;
  uint8_t number_of_banks;
  uint8_t memory_model;
  uint8_t bank_size;
  uint8_t number_of_image_pages;
  uint8_t reserved;

  /* Direct Color fields (required for direct/6 and YUV/7 memory models).  */
  uint8_t red_mask_size;
  uint8_t red_field_position;
  uint8_t green_mask_size;
  uint8_t green_field_position;
  uint8_t blue_mask_size;
  uint8_t blue_field_position;
  uint8_t rsvd_mask_size;
  uint8_t rsvd_field_position;
  uint8_t direct_color_mode_info;

  /* Mandatory information for VBE 2.0 and above.  */
  uint32_t phys_base_addr;
  uint32_t reserved2;
  uint16_t reserved3;

  /* Mandatory information for VBE 3.0 and above.  */
  uint16_t lin_bytes_per_scan_line;
  uint8_t bnk_number_of_image_pages;
  uint8_t lin_number_of_image_pages;
  uint8_t lin_red_mask_size;
  uint8_t lin_red_field_position;
  uint8_t lin_green_mask_size;
  uint8_t lin_green_field_position;
  uint8_t lin_blue_mask_size;
  uint8_t lin_blue_field_position;
  uint8_t lin_rsvd_mask_size;
  uint8_t lin_rsvd_field_position;
  uint32_t max_pixel_clock;

  /* Reserved field to make structure to be 256 bytes long, VESA BIOS
     Extension 3.0 Specification says to reserve 189 bytes here but
     that doesn't make structure to be 256 bytes.  So additional one is
     added here.  */
  uint8_t reserved4[190];
} __packed vbe_mode_info_block_t;

struct vbe_crtc_info {
    uint16_t horiz_total;
    uint16_t horiz_sync_start;
    uint16_t horiz_sync_end;
    uint16_t vert_total;
    uint16_t vert_sync_start;
    uint16_t vert_sync_end;
    uint8_t flags;
    uint32_t pixclock;
    uint16_t refresh_rate;
    uint8_t reserved[40];
} __packed;

/* VBE Return Status Info */
/* AL */
#define VBE_RETURN_STATUS_SUPPORTED                      0x4F
#define VBE_RETURN_STATUS_UNSUPPORTED                    0x00
/* AH */
#define VBE_RETURN_STATUS_SUCCESSFULL                    0x00
#define VBE_RETURN_STATUS_FAILED                         0x01
#define VBE_RETURN_STATUS_NOT_SUPPORTED                  0x02
#define VBE_RETURN_STATUS_INVALID                        0x03

/* VBE Mode Numbers */

#define VBE_MODE_VESA_DEFINED                            0x0100
#define VBE_MODE_REFRESH_RATE_USE_CRTC                   0x0800
#define VBE_MODE_LINEAR_FRAME_BUFFER                     0x4000
#define VBE_MODE_PRESERVE_DISPLAY_MEMORY                 0x8000

#define VBE_VESA_MODE_END_OF_LIST                        0xFFFF

/* Capabilities */

#define VBE_CAPABILITY_8BIT_DAC                          0x0001
#define VBE_CAPABILITY_NOT_VGA_COMPATIBLE                0x0002
#define VBE_CAPABILITY_RAMDAC_USE_BLANK_BIT              0x0004
#define VBE_CAPABILITY_STEREOSCOPIC_SUPPORT              0x0008
#define VBE_CAPABILITY_STEREO_VIA_VESA_EVC               0x0010

/* Mode Attributes */

#define VBE_MODE_ATTRIBUTE_SUPPORTED                     0x0001
#define VBE_MODE_ATTRIBUTE_EXTENDED_INFORMATION_AVAILABLE  0x0002
#define VBE_MODE_ATTRIBUTE_TTY_BIOS_SUPPORT              0x0004
#define VBE_MODE_ATTRIBUTE_COLOR_MODE                    0x0008
#define VBE_MODE_ATTRIBUTE_GRAPHICS_MODE                 0x0010
#define VBE_MODE_ATTRIBUTE_NOT_VGA_COMPATIBLE            0x0020
#define VBE_MODE_ATTRIBUTE_NO_VGA_COMPATIBLE_WINDOW      0x0040
#define VBE_MODE_ATTRIBUTE_LINEAR_FRAME_BUFFER_MODE      0x0080
#define VBE_MODE_ATTRIBUTE_DOUBLE_SCAN_MODE              0x0100
#define VBE_MODE_ATTRIBUTE_INTERLACE_MODE                0x0200
#define VBE_MODE_ATTRIBUTE_HARDWARE_TRIPLE_BUFFER        0x0400
#define VBE_MODE_ATTRIBUTE_HARDWARE_STEREOSCOPIC_DISPLAY 0x0800
#define VBE_MODE_ATTRIBUTE_DUAL_DISPLAY_START_ADDRESS    0x1000

#define VBE_MODE_ATTTRIBUTE_LFB_ONLY                     ( VBE_MODE_ATTRIBUTE_NO_VGA_COMPATIBLE_WINDOW | VBE_MODE_ATTRIBUTE_LINEAR_FRAME_BUFFER_MODE )

/* Window attributes */

#define VBE_WINDOW_ATTRIBUTE_RELOCATABLE                 0x01
#define VBE_WINDOW_ATTRIBUTE_READABLE                    0x02
#define VBE_WINDOW_ATTRIBUTE_WRITEABLE                   0x04

/* Memory model */

#define VBE_MEMORYMODEL_TEXT_MODE                        0x00
#define VBE_MEMORYMODEL_CGA_GRAPHICS                     0x01
#define VBE_MEMORYMODEL_HERCULES_GRAPHICS                0x02
#define VBE_MEMORYMODEL_PLANAR                           0x03
#define VBE_MEMORYMODEL_PACKED_PIXEL                     0x04
#define VBE_MEMORYMODEL_NON_CHAIN_4_256                  0x05
#define VBE_MEMORYMODEL_DIRECT_COLOR                     0x06
#define VBE_MEMORYMODEL_YUV                              0x07

/* DirectColorModeInfo */

#define VBE_DIRECTCOLOR_COLOR_RAMP_PROGRAMMABLE          0x01
#define VBE_DIRECTCOLOR_RESERVED_BITS_AVAILABLE          0x02

#endif
