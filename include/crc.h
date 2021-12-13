/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRC_H_
#define _CRC_H_

#include <types.h>

extern uint8_t crc4(uint8_t *src, int len, uint8_t crc);
extern uint8_t crc8(uint8_t *src, int len, uint8_t crc);
extern uint16_t crc16(uint8_t *src, int len, uint16_t crc);
extern uint32_t crc32(const uint8_t *s, int len, uint32_t crc);

#endif  /* _CRC_H_ */
