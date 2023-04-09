/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <lib.h>

#ifdef CONFIG_PACK_GZIP
#include "decompress_inflate.c"
#endif

void extract_kernel(void *output_start, void *input_start, unsigned long len)
{
    pr_boot("decompression info:\n");
    pr_boot("  input: %p\n", input_start);
    pr_boot("  output: %p\n", output_start);
    pr_boot("  length: 0x%x\n", len);

#ifdef CONFIG_PACK_KERNEL
    state ret;
    ret = decompress(input_start, len, NULL, NULL, output_start, 0, NULL);
    if (ret)
        panic("failed to decompressor kernel");
#else
    memcpy(output_start, input_start, len);
#endif

    kernel_check(output_start);
}
