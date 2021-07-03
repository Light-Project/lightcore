/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_BYTEORDER_BIG_ENDIAN_H
#define _UAPI_BYTEORDER_BIG_ENDIAN_H

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif
#ifndef __BIG_ENDIAN_BITFIELD
#define __BIG_ENDIAN_BITFIELD
#endif

#include <lightcore/types.h>
#include <lightcore/swab.h>

#define __constant_htonl(x) ((__force __be32)(uint32_t)(x))
#define __constant_ntohl(x) ((__force uint32_t)(__be32)(x))
#define __constant_htons(x) ((__force __be16)(uint16_t)(x))
#define __constant_ntohs(x) ((__force uint16_t)(__be16)(x))
#define __constant_cpu_to_le64(x) ((__force __le64)___constant_swab64((x)))
#define __constant_le64_to_cpu(x) ___constant_swab64((__force uint64_t)(__le64)(x))
#define __constant_cpu_to_le32(x) ((__force __le32)___constant_swab32((x)))
#define __constant_le32_to_cpu(x) ___constant_swab32((__force uint32_t)(__le32)(x))
#define __constant_cpu_to_le16(x) ((__force __le16)___constant_swab16((x)))
#define __constant_le16_to_cpu(x) ___constant_swab16((__force uint16_t)(__le16)(x))
#define __constant_cpu_to_be64(x) ((__force __be64)(uint64_t)(x))
#define __constant_be64_to_cpu(x) ((__force uint64_t)(__be64)(x))
#define __constant_cpu_to_be32(x) ((__force __be32)(uint32_t)(x))
#define __constant_be32_to_cpu(x) ((__force uint32_t)(__be32)(x))
#define __constant_cpu_to_be16(x) ((__force __be16)(uint16_t)(x))
#define __constant_be16_to_cpu(x) ((__force uint16_t)(__be16)(x))
#define __cpu_to_le64(x) ((__force __le64)__swab64((x)))
#define __le64_to_cpu(x) __swab64((__force uint64_t)(__le64)(x))
#define __cpu_to_le32(x) ((__force __le32)__swab32((x)))
#define __le32_to_cpu(x) __swab32((__force uint32_t)(__le32)(x))
#define __cpu_to_le16(x) ((__force __le16)__swab16((x)))
#define __le16_to_cpu(x) __swab16((__force uint16_t)(__le16)(x))
#define __cpu_to_be64(x) ((__force __be64)(uint64_t)(x))
#define __be64_to_cpu(x) ((__force uint64_t)(__be64)(x))
#define __cpu_to_be32(x) ((__force __be32)(uint32_t)(x))
#define __be32_to_cpu(x) ((__force uint32_t)(__be32)(x))
#define __cpu_to_be16(x) ((__force __be16)(uint16_t)(x))
#define __be16_to_cpu(x) ((__force uint16_t)(__be16)(x))

static __always_inline __le64 __cpu_to_le64p(const uint64_t *p)
{
	return (__force __le64)__swab64p(p);
}
static __always_inline uint64_t __le64_to_cpup(const __le64 *p)
{
	return __swab64p((uint64_t *)p);
}
static __always_inline __le32 __cpu_to_le32p(const uint32_t *p)
{
	return (__force __le32)__swab32p(p);
}
static __always_inline uint32_t __le32_to_cpup(const __le32 *p)
{
	return __swab32p((uint32_t *)p);
}
static __always_inline __le16 __cpu_to_le16p(const uint16_t *p)
{
	return (__force __le16)__swab16p(p);
}
static __always_inline uint16_t __le16_to_cpup(const __le16 *p)
{
	return __swab16p((uint16_t *)p);
}
static __always_inline __be64 __cpu_to_be64p(const uint64_t *p)
{
	return (__force __be64)*p;
}
static __always_inline uint64_t __be64_to_cpup(const __be64 *p)
{
	return (__force uint64_t)*p;
}
static __always_inline __be32 __cpu_to_be32p(const uint32_t *p)
{
	return (__force __be32)*p;
}
static __always_inline uint32_t __be32_to_cpup(const __be32 *p)
{
	return (__force uint32_t)*p;
}
static __always_inline __be16 __cpu_to_be16p(const uint16_t *p)
{
	return (__force __be16)*p;
}
static __always_inline uint16_t __be16_to_cpup(const __be16 *p)
{
	return (__force uint16_t)*p;
}
#define cpu_to_le64s(x) swab64s((x))
#define le64_to_cpus(x) swab64s((x))
#define cpu_to_le32s(x) swab32s((x))
#define le32_to_cpus(x) swab32s((x))
#define cpu_to_le16s(x) swab16s((x))
#define le16_to_cpus(x) swab16s((x))
#define cpu_to_be64s(x) do { (void)(x); } while (0)
#define be64_to_cpus(x) do { (void)(x); } while (0)
#define cpu_to_be32s(x) do { (void)(x); } while (0)
#define be32_to_cpus(x) do { (void)(x); } while (0)
#define cpu_to_be16s(x) do { (void)(x); } while (0)
#define be16_to_cpus(x) do { (void)(x); } while (0)


#endif /* _UAPI_LINUX_BYTEORDER_BIG_ENDIAN_H */
