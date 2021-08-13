#include <lib.h>
#include <crc_table.h>

static uint32_t crc32(const uint8_t *src, int len, uint32_t crc)
{
    while (len--)
        crc = (crc >> 8) ^ crc32_table[(crc & 0xff ) ^ *src++];
    return crc;
}

void kernel_check(void *addr)
{
    struct boot_head *boot_head = (struct boot_head *)addr;
    uint32_t crc32new, crc32old;
    uint32_t size = boot_head->size;
    
    pr_boot("Verification info:\n");
    pr_boot("    start: 0x%x\n", addr);
    pr_boot("    ksize: %ld\n", size);
    
    if(!strcmp((char *)&boot_head->magic, "lightcore!"))
        pr_boot("kernel magic correct\n");
    else
        panic("can't find kernel!\n");
    
    crc32old = boot_head->crc;
    crc32new = crc32((uint8_t *)(boot_head + 1), size, 0xffffffff);
    if(crc32old == crc32new)
        pr_boot("kernel crc32 correct 0x%x\n", crc32new);
    else
        panic("crc error 0x%x->0x%x\n", crc32old, crc32new);
} 
