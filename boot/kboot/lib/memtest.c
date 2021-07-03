#include <kboot.h>

static char tst[4] = {0x55, 0xaa, 0x00, 0xff};

void memtest(void *mem_start, size_t size)
{
    char *mem = mem_start;
    pr_boot("memtest: %x - %x\n\r", 
            (size_t)mem_start, (size_t)mem_start + size);
    
    while(size--)
    {
        for(unsigned char i = 0; i < 5; i++) {
            *mem = tst[i];
            if(*mem == tst[i])
                goto fail;
        }
        ++mem;
    }
    pr_boot("    no errot");
    return;
fail:
    panic("    badblock find: %x\n\r", mem);
}
