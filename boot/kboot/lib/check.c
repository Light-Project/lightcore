#include <lib.h>
#include <crc_table.h>

static uint32_t crc32_byte(char c, uint32_t crc)
{
    return crc32_table[(crc ^ c) & 0xff] ^ (crc >> 8);
}

static uint32_t crc32(const char *s, int len, uint32_t crc)
{
    while (len--)
        crc = crc32_byte(*s++, crc);
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
    crc32new = crc32((const char *)(boot_head + 1), size, 0xffffffff);
    if(crc32old == crc32new)
        pr_boot("kernel crc32 correct 0x%x\n", crc32new);
    else
        panic("crc error 0x%x->0x%x\n", crc32new, crc32old);
} 
