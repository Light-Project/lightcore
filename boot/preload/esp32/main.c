#include <boot.h>
#include <asm/io.h>
#include <bits.h>
#include <asm-generic/header.h>
#include <crc-table.h>

static uint32_t crc32(const uint32_t *s, int len, uint32_t crc)
{
    uint32_t buf;
    len /= 4;
    while (len--){
        buf = readl((void *)s++);
        for(char t=0; t<4; t++){
            crc = crc32_table[(crc ^ (buf & 0xff)) & 0xff] ^ (crc >> 8);
            buf >>= 8;
        }
    }
    return crc;
}

// Check kernel integrity
static void kernel_check(void *addr)
{
    struct boot_head *boot_head = addr;
    uint32_t crc32new = 0, crc32old = 0;
    uint8_t cmp[17];
    uint32_t size;

    pr_boot("Check addr: 0x%x\n", addr);

    size = readl(&boot_head->size);
    pr_boot("Kernel size: 0x%d\n", size);

    for(uint8_t c = 0; c <= 4; c++)
        cmp[c*4] = readl(&boot_head->magic[c*4]);
    cmp[17] = '\0';
    if(!strcmp((char *)&cmp, "lightcore!"))
        pr_boot("kernel magic correct\n");
    else
        panic("can't find kernel!\n");

    crc32old = readl(&boot_head->crc);
    crc32new = crc32((uint32_t *)(boot_head + 1), size, 0xffffffff);
    if(crc32old == crc32new)
        pr_boot("kernel crc32 correct 0x%x \n", crc32new);
    else
        panic("crc error 0x%x->0x%x!\n", crc32old, crc32new);

}

#define rtc_base 0x3ff48000
#define timg_base 0x3ff5F000
static void wdt_disable()
{
    uint32_t val;

    /* Disable rtc wdt */
    writel((void *)rtc_base + 0xa4, 0x50d83aa1);
    writel((void *)rtc_base + 0xa0, BIT(31));
    writel((void *)rtc_base + 0x8c, 0x00);
    writel((void *)rtc_base + 0xa4, 0x00);

    /* Disable timg wdt */
    val = readl((void *)timg_base + 0x48);
    val &= BIT(31);
    writel((void *)timg_base + 0x48, val);
}

#define dport_base 0x3ff00000
#define apbctl_base 0x3ff66000
void cpu_freq()
{
    uint32_t val;

    val = readl((void *)rtc_base + 0x70);
    val = (val & (~0x18000000)) | 0x8000000;
    writel((void *)rtc_base + 0x70, val);

    writel((void *)dport_base + 0x3c, 2);
    writel((void *)apbctl_base + 0x08, APB_CLK_FREQ / REF_CLK_FREQ - 1);

}

void main()
{
    wdt_disable();
    cpu_freq();

    uart_init();
    pr_init(uart_print);

//     timer_init();

    xip_init();
    pr_boot("Mmap kernel to memory\n");

    kernel_check((void *)CONFIG_XIP_BASE);

    pr_boot("Start kernel...\n");
//     pr_boot("total boot time: %dms\n", time_read());

}
