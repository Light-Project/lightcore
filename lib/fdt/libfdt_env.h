/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef LIBFDT_ENV_H
#define LIBFDT_ENV_H

#include <limits.h>         /* For INT_MAX */
#include <string.h>
#include <stddef.h>

#include <lightcore/asm/byteorder.h>  /* For be32_to_cpu */

typedef be16 fdt16_t;
typedef be32 fdt32_t;
typedef be64 fdt64_t;

#define fdt32_to_cpu(x) be32_to_cpu(x)
#define cpu_to_fdt32(x) cpu_to_be32(x)
#define fdt64_to_cpu(x) be64_to_cpu(x)
#define cpu_to_fdt64(x) cpu_to_be64(x)

#endif /* LIBFDT_ENV_H */
