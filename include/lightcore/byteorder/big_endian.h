/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_
#define _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_

#ifndef __BIG_ENDIAN__
#define __BIG_ENDIAN__
#endif

#include <lightcore/types.h>
#include <lightcore/swab.h>

#define constant_htons(x) ((be16)(uint16_t)(x))
#define constant_htonl(x) ((be32)(uint32_t)(x))
#define constant_ntohl(x) ((uint32_t)(be32)(x))
#define constant_ntohs(x) ((uint16_t)(be16)(x))

#define constant_cpu_to_le16(x) ((le16)constant_swab16((x)))
#define constant_cpu_to_le32(x) ((le32)constant_swab32((x)))
#define constant_cpu_to_le64(x) ((le64)constant_swab64((x)))
#define constant_le16_to_cpu(x) constant_swab16((uint16_t)(le16)(x))
#define constant_le32_to_cpu(x) constant_swab32((uint32_t)(le32)(x))
#define constant_le64_to_cpu(x) constant_swab64((uint64_t)(le64)(x))

#define constant_cpu_to_be16(x) ((be16)(uint16_t)(x))
#define constant_cpu_to_be32(x) ((be32)(uint32_t)(x))
#define constant_cpu_to_be64(x) ((be64)(uint64_t)(x))
#define constant_be16_to_cpu(x) ((uint16_t)(be16)(x))
#define constant_be32_to_cpu(x) ((uint32_t)(be32)(x))
#define constant_be64_to_cpu(x) ((uint64_t)(be64)(x))

#define cpu_to_le16(x) ((le16)swab16((x)))
#define cpu_to_le32(x) ((le32)swab32((x)))
#define cpu_to_le64(x) ((le64)swab64((x)))
#define le16_to_cpu(x) swab16((uint16_t)(le16)(x))
#define le32_to_cpu(x) swab32((uint32_t)(le32)(x))
#define le64_to_cpu(x) swab64((uint64_t)(le64)(x))

#define be16_to_cpu(x) ((uint16_t)(be16)(x))
#define be32_to_cpu(x) ((uint32_t)(be32)(x))
#define be64_to_cpu(x) ((uint64_t)(be64)(x))
#define cpu_to_be16(x) ((be16)(uint16_t)(x))
#define cpu_to_be32(x) ((be32)(uint32_t)(x))
#define cpu_to_be64(x) ((be64)(uint64_t)(x))

#define cpu_to_le16s(x) swab16s((x))
#define cpu_to_le32s(x) swab32s((x))
#define cpu_to_le64s(x) swab64s((x))
#define le16_to_cpus(x) swab16s((x))
#define le32_to_cpus(x) swab32s((x))
#define le64_to_cpus(x) swab64s((x))

#define cpu_to_be16s(x) do { (void)(x); } while (0)
#define cpu_to_be32s(x) do { (void)(x); } while (0)
#define cpu_to_be64s(x) do { (void)(x); } while (0)
#define be16_to_cpus(x) do { (void)(x); } while (0)
#define be32_to_cpus(x) do { (void)(x); } while (0)
#define be64_to_cpus(x) do { (void)(x); } while (0)

static inline le16 cpu_to_le16p(const uint16_t *p)
{
    return (le16)swab16p(p);
}

static inline le32 cpu_to_le32p(const uint32_t *p)
{
    return (le32)swab32p(p);
}

static inline le64 cpu_to_le64p(const uint64_t *p)
{
    return (le64)swab64p(p);
}

static inline uint16_t le16_to_cpup(const le16 *p)
{
    return swab16p((uint16_t *)p);
}

static inline uint32_t le32_to_cpup(const le32 *p)
{
    return swab32p((uint32_t *)p);
}

static inline uint64_t le64_to_cpup(const le64 *p)
{
    return swab64p((uint64_t *)p);
}

static inline be16 cpu_to_be16p(const uint16_t *p)
{
    return (be16)*p;
}

static inline be32 cpu_to_be32p(const uint32_t *p)
{
    return (be32)*p;
}

static inline be64 cpu_to_be64p(const uint64_t *p)
{
    return (be64)*p;
}

static inline uint16_t be16_to_cpup(const be16 *p)
{
    return (uint16_t)*p;
}

static inline uint32_t be32_to_cpup(const be32 *p)
{
    return (uint32_t)*p;
}

static inline uint64_t be64_to_cpup(const be64 *p)
{
    return (uint64_t)*p;
}

#endif /* _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_ */
