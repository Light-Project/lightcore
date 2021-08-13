/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_
#define _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif
#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif

#include <lightcore/types.h>
#include <lightcore/swab.h>

#define constant_htonl(x) ((__force be32)constant_swab32((x)))
#define constant_ntohl(x) constant_swab32((__force be32)(x))
#define constant_htons(x) ((__force be16)constant_swab16((x)))
#define constant_ntohs(x) constant_swab16((__force be16)(x))
#define constant_cpu_to_le64(x) ((__force le64)(u64)(x))
#define constant_le64_to_cpu(x) ((__force u64)(le64)(x))
#define constant_cpu_to_le32(x) ((__force le32)(u32)(x))
#define constant_le32_to_cpu(x) ((__force u32)(le32)(x))
#define constant_cpu_to_le16(x) ((__force le16)(u16)(x))
#define constant_le16_to_cpu(x) ((__force u16)(le16)(x))
#define constant_cpu_to_be64(x) ((__force be64)constant_swab64((x)))
#define constant_be64_to_cpu(x) constant_swab64((__force u64)(be64)(x))
#define constant_cpu_to_be32(x) ((__force be32)constant_swab32((x)))
#define constant_be32_to_cpu(x) constant_swab32((__force u32)(be32)(x))
#define constant_cpu_to_be16(x) ((__force be16)constant_swab16((x)))
#define constant_be16_to_cpu(x) constant_swab16((__force u16)(be16)(x))
#define cpu_to_le64(x) ((__force le64)(u64)(x))
#define le64_to_cpu(x) ((__force u64)(le64)(x))
#define cpu_to_le32(x) ((__force le32)(u32)(x))
#define le32_to_cpu(x) ((__force u32)(le32)(x))
#define cpu_to_le16(x) ((__force le16)(u16)(x))
#define le16_to_cpu(x) ((__force u16)(le16)(x))
#define cpu_to_be64(x) ((__force be64)swab64((x)))
#define be64_to_cpu(x) swab64((__force u64)(be64)(x))
#define cpu_to_be32(x) ((__force be32)swab32((x)))
#define be32_to_cpu(x) swab32((__force u32)(be32)(x))
#define cpu_to_be16(x) ((__force be16)swab16((x)))
#define be16_to_cpu(x) swab16((__force u16)(be16)(x))

static __always_inline le64 cpu_to_le64p(const uint64_t *p)
{
	return (__force le64)*p;
}
static __always_inline uint64_t le64_to_cpup(const le64 *p)
{
	return (__force uint64_t)*p;
}
static __always_inline le32 cpu_to_le32p(const uint32_t *p)
{
	return (__force le32)*p;
}
static __always_inline uint32_t le32_to_cpup(const le32 *p)
{
	return (__force uint32_t)*p;
}
static __always_inline le16 cpu_to_le16p(const uint16_t *p)
{
	return (__force le16)*p;
}
static __always_inline uint16_t le16_to_cpup(const le16 *p)
{
	return (__force uint16_t)*p;
}
static __always_inline be64 cpu_to_be64p(const uint64_t *p)
{
	return (__force be64)swab64p(p);
}
static __always_inline uint64_t be64_to_cpup(const be64 *p)
{
	return swab64p((uint64_t *)p);
}
static __always_inline be32 cpu_to_be32p(const uint32_t *p)
{
	return (__force be32)swab32p(p);
}
static __always_inline uint32_t be32_to_cpup(const be32 *p)
{
	return swab32p((uint32_t *)p);
}
static __always_inline be16 cpu_to_be16p(const uint16_t *p)
{
	return (__force be16)swab16p(p);
}
static __always_inline uint16_t be16_to_cpup(const be16 *p)
{
	return swab16p((uint16_t *)p);
}

#define cpu_to_le16s(x) do { (void)(x); } while (0)
#define le16_to_cpus(x) do { (void)(x); } while (0)
#define cpu_to_le32s(x) do { (void)(x); } while (0)
#define le32_to_cpus(x) do { (void)(x); } while (0)
#define cpu_to_le64s(x) do { (void)(x); } while (0)
#define le64_to_cpus(x) do { (void)(x); } while (0)

#define cpu_to_be16s(x) swab16s((x))
#define be16_to_cpus(x) swab16s((x))
#define cpu_to_be32s(x) swab32s((x))
#define be32_to_cpus(x) swab32s((x))
#define cpu_to_be64s(x) swab64s((x))
#define be64_to_cpus(x) swab64s((x))


#endif /* _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_ */
