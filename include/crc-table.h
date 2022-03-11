/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRC_TABLE_H_
#define _CRC_TABLE_H_

const uint8_t crc4_table[16] = {
    0x0, 0x7, 0xe, 0x9, 0xb, 0xc, 0x5, 0x2,
    0x1, 0x6, 0xf, 0x8, 0xa, 0xd, 0x4, 0x3,
};

static const uint8_t crc8_table[256] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
    0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
    0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5,
    0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85,
    0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
    0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
    0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2,
    0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32,
    0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
    0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
    0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c,
    0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec,
    0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
    0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
    0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c,
    0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b,
    0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
    0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
    0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb,
    0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb,
    0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3,
};

static const uint16_t crc16_table[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

static const uint32_t crc32_table[256] = {
    0x000000000U, 0x77073096U, 0xee0e612cU, 0x990951baU,
    0x076dc419lU, 0x706af48fU, 0xe963a535U, 0x9e6495a3U,
    0x0edb8832lU, 0x79dcb8a4U, 0xe0d5e91eU, 0x97d2d988U,
    0x09b64c2blU, 0x7eb17cbdU, 0xe7b82d07U, 0x90bf1d91U,
    0x1db71064lU, 0x6ab020f2U, 0xf3b97148U, 0x84be41deU,
    0x1adad47dlU, 0x6ddde4ebU, 0xf4d4b551U, 0x83d385c7U,
    0x136c9856lU, 0x646ba8c0U, 0xfd62f97aU, 0x8a65c9ecU,
    0x14015c4flU, 0x63066cd9U, 0xfa0f3d63U, 0x8d080df5U,
    0x3b6e20c8lU, 0x4c69105eU, 0xd56041e4U, 0xa2677172U,
    0x3c03e4d1lU, 0x4b04d447U, 0xd20d85fdU, 0xa50ab56bU,
    0x35b5a8falU, 0x42b2986cU, 0xdbbbc9d6U, 0xacbcf940U,
    0x32d86ce3lU, 0x45df5c75U, 0xdcd60dcfU, 0xabd13d59U,
    0x26d930aclU, 0x51de003aU, 0xc8d75180U, 0xbfd06116U,
    0x21b4f4b5lU, 0x56b3c423U, 0xcfba9599U, 0xb8bda50fU,
    0x2802b89elU, 0x5f058808U, 0xc60cd9b2U, 0xb10be924U,
    0x2f6f7c87lU, 0x58684c11U, 0xc1611dabU, 0xb6662d3dU,
    0x76dc4190lU, 0x01db7106U, 0x98d220bcU, 0xefd5102aU,
    0x71b18589lU, 0x06b6b51fU, 0x9fbfe4a5U, 0xe8b8d433U,
    0x7807c9a2lU, 0x0f00f934U, 0x9609a88eU, 0xe10e9818U,
    0x7f6a0dbblU, 0x086d3d2dU, 0x91646c97U, 0xe6635c01U,
    0x6b6b51f4lU, 0x1c6c6162U, 0x856530d8U, 0xf262004eU,
    0x6c0695edlU, 0x1b01a57bU, 0x8208f4c1U, 0xf50fc457U,
    0x65b0d9c6lU, 0x12b7e950U, 0x8bbeb8eaU, 0xfcb9887cU,
    0x62dd1ddflU, 0x15da2d49U, 0x8cd37cf3U, 0xfbd44c65U,
    0x4db26158lU, 0x3ab551ceU, 0xa3bc0074U, 0xd4bb30e2U,
    0x4adfa541lU, 0x3dd895d7U, 0xa4d1c46dU, 0xd3d6f4fbU,
    0x4369e96alU, 0x346ed9fcU, 0xad678846U, 0xda60b8d0U,
    0x44042d73lU, 0x33031de5U, 0xaa0a4c5fU, 0xdd0d7cc9U,
    0x5005713clU, 0x270241aaU, 0xbe0b1010U, 0xc90c2086U,
    0x5768b525lU, 0x206f85b3U, 0xb966d409U, 0xce61e49fU,
    0x5edef90elU, 0x29d9c998U, 0xb0d09822U, 0xc7d7a8b4U,
    0x59b33d17lU, 0x2eb40d81U, 0xb7bd5c3bU, 0xc0ba6cadU,
    0xedb88320lU, 0x9abfb3b6U, 0x03b6e20cU, 0x74b1d29aU,
    0xead54739lU, 0x9dd277afU, 0x04db2615U, 0x73dc1683U,
    0xe3630b12lU, 0x94643b84U, 0x0d6d6a3eU, 0x7a6a5aa8U,
    0xe40ecf0blU, 0x9309ff9dU, 0x0a00ae27U, 0x7d079eb1U,
    0xf00f9344lU, 0x8708a3d2U, 0x1e01f268U, 0x6906c2feU,
    0xf762575dlU, 0x806567cbU, 0x196c3671U, 0x6e6b06e7U,
    0xfed41b76lU, 0x89d32be0U, 0x10da7a5aU, 0x67dd4accU,
    0xf9b9df6flU, 0x8ebeeff9U, 0x17b7be43U, 0x60b08ed5U,
    0xd6d6a3e8lU, 0xa1d1937eU, 0x38d8c2c4U, 0x4fdff252U,
    0xd1bb67f1lU, 0xa6bc5767U, 0x3fb506ddU, 0x48b2364bU,
    0xd80d2bdalU, 0xaf0a1b4cU, 0x36034af6U, 0x41047a60U,
    0xdf60efc3lU, 0xa867df55U, 0x316e8eefU, 0x4669be79U,
    0xcb61b38clU, 0xbc66831aU, 0x256fd2a0U, 0x5268e236U,
    0xcc0c7795lU, 0xbb0b4703U, 0x220216b9U, 0x5505262fU,
    0xc5ba3bbelU, 0xb2bd0b28U, 0x2bb45a92U, 0x5cb36a04U,
    0xc2d7ffa7lU, 0xb5d0cf31U, 0x2cd99e8bU, 0x5bdeae1dU,
    0x9b64c2b0lU, 0xec63f226U, 0x756aa39cU, 0x026d930aU,
    0x9c0906a9lU, 0xeb0e363fU, 0x72076785U, 0x05005713U,
    0x95bf4a82lU, 0xe2b87a14U, 0x7bb12baeU, 0x0cb61b38U,
    0x92d28e9blU, 0xe5d5be0dU, 0x7cdcefb7U, 0x0bdbdf21U,
    0x86d3d2d4lU, 0xf1d4e242U, 0x68ddb3f8U, 0x1fda836eU,
    0x81be16cdlU, 0xf6b9265bU, 0x6fb077e1U, 0x18b74777U,
    0x88085ae6lU, 0xff0f6a70U, 0x66063bcaU, 0x11010b5cU,
    0x8f659efflU, 0xf862ae69U, 0x616bffd3U, 0x166ccf45U,
    0xa00ae278lU, 0xd70dd2eeU, 0x4e048354U, 0x3903b3c2U,
    0xa7672661lU, 0xd06016f7U, 0x4969474dU, 0x3e6e77dbU,
    0xaed16a4alU, 0xd9d65adcU, 0x40df0b66U, 0x37d83bf0U,
    0xa9bcae53lU, 0xdebb9ec5U, 0x47b2cf7fU, 0x30b5ffe9U,
    0xbdbdf21clU, 0xcabac28aU, 0x53b39330U, 0x24b4a3a6U,
    0xbad03605lU, 0xcdd70693U, 0x54de5729U, 0x23d967bfU,
    0xb3667a2elU, 0xc4614ab8U, 0x5d681b02U, 0x2a6f2b94U,
    0xb40bbe37lU, 0xc30c8ea1U, 0x5a05df1bU, 0x2d02ef8dU,
};

static inline uint8_t crc4_inline(uint8_t *src, int len, uint8_t crc)
{
    uint8_t tmp = crc;

    len *= 2;
    while (len--)
        tmp = crc4_table[(tmp & 0x0f) ^ *src++];

    return tmp ^ crc;
}

static inline uint8_t crc8_inline(uint8_t *src, int len, uint8_t crc)
{
    uint8_t tmp = crc;

    while (len--)
        tmp = crc8_table[(tmp & 0xff) ^ *src++];

    return tmp ^ crc;
}

static inline uint16_t crc16_inline(uint8_t *src, int len, uint16_t crc)
{
    uint32_t tmp = crc;

    while (len--)
        tmp = (tmp << 8) ^ crc16_table[(tmp & 0xff) ^ *src++];

    return tmp ^ crc;
}

static inline uint32_t crc32_inline(const uint8_t *src, int len, uint32_t crc)
{
    uint32_t tmp = crc;

    while (len--)
        tmp = (tmp >> 8) ^ crc32_table[(tmp & 0xff) ^ *src++];

    return tmp ^ crc;
}

#endif  /* _CRC_TABLE_H_ */