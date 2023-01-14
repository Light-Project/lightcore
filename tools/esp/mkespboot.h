/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _MKESPBOOT_H_
#define _MKESPBOOT_H_

#include <stdint.h>

enum ESP_symbol {
    // Commands supported by ESP8266 ROM bootloader
    ESP_FLASH_BEGIN         = 0x02,
    ESP_FLASH_DATA          = 0x03,
    ESP_FLASH_END           = 0x04,
    ESP_MEM_BEGIN           = 0x05,
    ESP_MEM_END             = 0x06,
    ESP_MEM_DATA            = 0x07,
    ESP_SYNC                = 0x08,
    ESP_WRITE_REG           = 0x09,
    ESP_READ_REG            = 0x0a,

    // Some comands supported by ESP32 ROM bootloader (or -8266 w/ stub)
    ESP_SPI_SET_PARAMS      = 0x0B,
    ESP_SPI_ATTACH          = 0x0D,
    ESP_READ_FLASH_SLOW     = 0x0e,  // ROM only, much slower than the stub flash read
    ESP_CHANGE_BAUDRATE     = 0x0F,
    ESP_FLASH_DEFL_BEGIN    = 0x10,
    ESP_FLASH_DEFL_DATA     = 0x11,
    ESP_FLASH_DEFL_END      = 0x12,
    ESP_SPI_FLASH_MD5       = 0x13,

    // Commands supported by ESP32-S2/S3/C3 ROM bootloader only
    ESP_GET_SECURITY_INFO   = 0x14,

    // Some commands supported by stub only
    ESP_ERASE_FLASH         = 0xD0,
    ESP_ERASE_REGION        = 0xD1,
    ESP_READ_FLASH          = 0xD2,
    ESP_RUN_USER_CODE       = 0xD3,

    // Flash encryption encrypted data command
    ESP_FLASH_ENCRYPT_DATA  = 0xD4,

    // Response code(s) sent by ROM
    ROM_INVALID_RECV_MSG    = 0x05,   // response if an invalid message is received

    // Maximum block sized for RAM and Flash writes, respectively.
    ESP_RAM_BLOCK           = 0x1800,

    FLASH_WRITE_SIZE        = 0x400,

    // Default baudrate. The ROM auto-bauds, so we can use more or less whatever we want.
    ESP_ROM_BAUD            = 115200,

    // First byte of the application image
    ESP_IMAGE_MAGIC         = 0xe9,

    // Initial state for the checksum routine
    ESP_CHECKSUM_MAGIC      = 0xef,
};

typedef struct ESP_head {
    uint8_t magic;
    uint8_t segment_count;
    uint8_t flags0;
    uint8_t flags1;
    uint32_t entry;
    uint32_t load;
    uint32_t size;
    uint8_t seg;
} __attribute__((packed)) ESP_head_t;

typedef struct ESP_node {
    uint8_t magic;
    uint8_t flags0;
    uint8_t flags1;
    uint32_t entry;
    uint32_t load;
    uint32_t size;
} __attribute__((packed))  ESP_node_t;

typedef struct ESP_check {
    uint32_t Res0;
    uint32_t Res1;
    uint32_t Res2;
    uint16_t Res3;
    uint8_t Res4;
    uint8_t checksum;
} __attribute__((packed))  ESP_check_t;

#endif
