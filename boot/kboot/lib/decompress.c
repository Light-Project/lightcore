#include <lib.h>

#ifdef CONFIG_PACK_GZIP
#include "decompress_inflate.c"
#endif

#ifdef CONFIG_PACK_LZO
#include "../../../../lib/decompress_unlzo.c"
#endif

#ifdef CONFIG_PACK_LZMA
#include "../../../../lib/decompress_unlzma.c"
#endif

#ifdef CONFIG_PACK_XZ
#define memmove memmove
#define memcpy memcpy
#include "../../../../lib/decompress_unxz.c"
#endif

#ifdef CONFIG_PACK_LZ4
#include "../../../../lib/decompress_unlz4.c"
#endif

void extract_kernel(void *output_start, void *input_start, unsigned long len)
{
    pr_boot("Decompression info:\n");
    pr_boot("    input: 0x%x\n", input_start);
    pr_boot("    output: 0x%x\n", output_start);
    pr_boot("    length: %d\n", len);
    
#ifdef CONFIG_PACK_KERNEL   
    int ret;     
    ret = __decompress(input_start, len, NULL, NULL, output_start,
                        0, NULL, (void (*)(char *))panic);
    if (ret)
        panic("decompressor returned an error");
#else
    memcpy(output_start, input_start, len);
#endif
    kernel_check(output_start);
}
