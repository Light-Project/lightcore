/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_
#define _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_

#ifndef __LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__
#endif

#include <lightcore/types.h>
#include <lightcore/swab.h>

#define constant_htons(x) ((__be16)constant_swab16((x)))
#define constant_htonl(x) ((__be32)constant_swab32((x)))
#define constant_ntohl(x) constant_swab32((__be32)(x))
#define constant_ntohs(x) constant_swab16((__be16)(x))

#define constant_cpu_to_le16(x) ((__le16)(__u16)(x))
#define constant_cpu_to_le32(x) ((__le32)(__u32)(x))
#define constant_cpu_to_le64(x) ((__le64)(__u64)(x))
#define constant_le16_to_cpu(x) ((__u16)(__le16)(x))
#define constant_le32_to_cpu(x) ((__u32)(__le32)(x))
#define constant_le64_to_cpu(x) ((__u64)(__le64)(x))

#define constant_cpu_to_be16(x) ((__be16)constant_swab16((x)))
#define constant_cpu_to_be32(x) ((__be32)constant_swab32((x)))
#define constant_cpu_to_be64(x) ((__be64)constant_swab64((x)))
#define constant_be16_to_cpu(x) constant_swab16((__u16)(__be16)(x))
#define constant_be32_to_cpu(x) constant_swab32((__u32)(__be32)(x))
#define constant_be64_to_cpu(x) constant_swab64((__u64)(__be64)(x))

#define cpu_to_le16(x) ((__le16)(__u16)(x))
#define cpu_to_le32(x) ((__le32)(__u32)(x))
#define cpu_to_le64(x) ((__le64)(__u64)(x))
#define le16_to_cpu(x) ((__u16)(__le16)(x))
#define le32_to_cpu(x) ((__u32)(__le32)(x))
#define le64_to_cpu(x) ((__u64)(__le64)(x))

#define cpu_to_be16(x) ((__be16)swab16((x)))
#define cpu_to_be32(x) ((__be32)swab32((x)))
#define cpu_to_be64(x) ((__be64)swab64((x)))
#define be16_to_cpu(x) swab16((__u16)(__be16)(x))
#define be32_to_cpu(x) swab32((__u32)(__be32)(x))
#define be64_to_cpu(x) swab64((__u64)(__be64)(x))

#define cpu_to_le16s(x) do { (void)(x); } while (0)
#define cpu_to_le32s(x) do { (void)(x); } while (0)
#define cpu_to_le64s(x) do { (void)(x); } while (0)
#define le16_to_cpus(x) do { (void)(x); } while (0)
#define le32_to_cpus(x) do { (void)(x); } while (0)
#define le64_to_cpus(x) do { (void)(x); } while (0)

#define cpu_to_be16s(x) swab16s((x))
#define cpu_to_be32s(x) swab32s((x))
#define cpu_to_be64s(x) swab64s((x))
#define be16_to_cpus(x) swab16s((x))
#define be32_to_cpus(x) swab32s((x))
#define be64_to_cpus(x) swab64s((x))

static __always_inline __le16 cpu_to_le16p(const __u16 *p)
{
    return (__le16)*p;
}

static __always_inline __le32 cpu_to_le32p(const __u32 *p)
{
    return (__le32)*p;
}

static __always_inline __le64 cpu_to_le64p(const __u64 *p)
{
    return (__le64)*p;
}

static __always_inline __u16 le16_to_cpup(const __le16 *p)
{
    return (__u16)*p;
}

static __always_inline __u32 le32_to_cpup(const __le32 *p)
{
    return (__u32)*p;
}

static __always_inline __u64 le64_to_cpup(const __le64 *p)
{
    return (__u64)*p;
}

static __always_inline __be16 cpu_to_be16p(const __u16 *p)
{
    return (__be16)swab16p(p);
}

static __always_inline __be32 cpu_to_be32p(const __u32 *p)
{
    return (__be32)swab32p(p);
}

static __always_inline __be64 cpu_to_be64p(const __u64 *p)
{
    return (__be64)swab64p(p);
}

static __always_inline __u16 be16_to_cpup(const __be16 *p)
{
    return swab16p((__u16 *)p);
}

static __always_inline __u32 be32_to_cpup(const __be32 *p)
{
    return swab32p((__u32 *)p);
}

static __always_inline __u64 be64_to_cpup(const __be64 *p)
{
    return swab64p((__u64 *)p);
}

#endif /* _LIGHTCORE_BYTEORDER_LITTLE_ENDIAN_H_ */
