/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_CRC_H_
#define _CRYPTO_CRC_H_

#include <types.h>

extern uint8_t crc4(const uint8_t *src, size_t len, uint8_t crc);
extern uint8_t crc8(const uint8_t *src, size_t len, uint8_t crc);
extern uint16_t crc16(const uint8_t *src, size_t len, uint16_t crc);
extern uint32_t crc32(const uint8_t *src, size_t len, uint32_t crc);

extern uint16_t crc_ccitt(const uint8_t *src, size_t len, uint16_t crc);
extern uint16_t crc_ccittf(const uint8_t *src, size_t len, uint16_t crc);
extern uint16_t crc_itut(const uint8_t *src, size_t len, uint16_t crc);
extern uint16_t crc_t10dif(const uint8_t *src, size_t len, uint16_t crc);

#endif  /* _CRYPTO_CRC_H_ */
