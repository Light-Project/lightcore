#include <stddef.h>
#include <chip/esp8266/gpio.h>
#include <chip/esp8266/iomux.h>
#include <driver/watchdog/watchdog-esp8266.h>
#include <asm-generic/header.h>
#include <crc/crc32_table.h>
#include <asm/io.h>
#include <boot.h>

extern void *_ld_dts_start;

static uint32_t crc32(const uint32_t *s, int len, uint32_t crc)
{
    uint32_t buf;
    len /= 4;
    while (len--){
        buf = readl_sync((void *)s++);
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
    
    pr_boot("Check addr: 0x%x\n\r", addr);
    
    size = readl_sync(&boot_head->size);
    pr_boot("Kernel size: 0x%d\n\r", size);
    
    for(uint8_t c = 0; c <= 4; c++)
        cmp[c*4] = readl_sync(&boot_head->magic[c*4]);
    cmp[17] = '\0';
    if(!strcmp((char *)&cmp, "lightcore!"))
        pr_boot("kernel magic correct\n\r");
    else
        panic("can't find kernel!\n\r");
    
    crc32old = readl_sync(&boot_head->crc);
    crc32new = crc32((uint32_t *)(boot_head + 1), size, 0xffffffff);
    if(crc32old == crc32new)
        pr_boot("kernel crc32 correct 0x%x \n\r", crc32new);
    else
        panic("crc error 0x%x->0x%x!\n\r", crc32old, crc32new);

}

#define ccu ((void *)0x3ff00014)
static void cpu_pll(char mul2)
{
    uint32_t val;
    val = readl_sync(ccu);
    val &= (mul2 | 0xfffffffe);
    val |= (mul2 & 0x01);
    writel_sync(ccu, val);
    pr_boot("CPU Speed: %dMhz\n\r", APB_CLK_FREQ * (mul2 + 1) / 1000000);
    pr_boot("BUS Speed: %dMhz\n\r", APB_CLK_FREQ / 1000000);
    pr_boot("OSC Speed: %dMhz\n\r", OSC_FREQ / 1000000);
}

void led_set(char out)
{
    ESP8266_IOMUX2->FUNL = 0;
    ESP8266_IOMUX2->FUNH = 0;

    ESP8266_GPIO->enable_w1ts |= 1<<2;
    
    if(out)
        ESP8266_GPIO->out_w1ts |= 1<<2;
    else
        ESP8266_GPIO->out_w1tc |= 1<<2;
}

static const char *reset_reason[] = {
    "No mean",
    "Power on reset",
    "External reset",
    "Software reset",
    "Watch-dog reset",
    "Deep-Sleep reset",
    "SLC reset",
};

void main()
{
    uint8_t reason;
    
    cpu_pll(1);

    uart_init();
    pr_init(uart_print);
    
    timer_init();

    led_set(0);
    
    reason = readb_sync((void *)(0x60000700 + 0x14));
    reason = min((uint8_t)ARRAY_SIZE(reset_reason), reason);
    pr_boot("Reset: %s\n\r", reset_reason[reason]);
    
    /* XIP can only map 1MiB space at the same time ! */
    
    /* arg0: Map address                     */
    /* 0: 0x000000 - 0x0fffff \              */
    /*                         \             */
    /*                           0x40200000  */
    /*                         /             */
    /* 1: 0x100000 - 0x1fffff /              */
    
    /* arg1: Flash offset                    */
    /* 0: 0x000000 - 0x1fffff                */
    /* 1: 0x200000 - 0x3fffff                */
    /* 2: 0x400000 - 0x5fffff                */
    /* 3: 0x600000 - 0x7fffff                */
    
    /* arg2: Xip cache size                  */
    /* 0: 16KiB                              */
    /* 1: 32KiB                              */
    
    Cache_Read_Disable();
    Cache_Read_Enable(0, 0, 1);
    pr_boot("Mmap kernel to memory\n\r");
    
    kernel_check((void *)CONFIG_XIP_BASE);
    
    pr_boot("Start kernel...\n\r");
    pr_boot("total boot time: %dms\n\r", time_read());
    
    led_set(1);
        
    timer_stop();
       
    kernel_start(NULL, &_ld_dts_start);
}
