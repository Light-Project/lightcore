#include "boot.h"
#include <size.h>
#include <chip/sunxif1.h>
#include <asm-generic/header.h>

static uint32_t check_device()
{
    if(*(uint32_t *)0x8 == 0x4c45462e)
        return 0;
    return 1;
}

static uint32_t boot_norflash()
{
    uint8_t buff[64];
    spi_init();
    nor_read_id(buff);
    
//     nor_read(buff,0x10000,64);
}
static void print_flash_info(const esp_image_header_t* phdr)
{
#if (BOOT_LOG_LEVEL >= BOOT_LOG_LEVEL_NOTICE)

    ESP_LOGD(TAG, "magic %02x", phdr->magic );
    ESP_LOGD(TAG, "segments %02x", phdr->segment_count );
    ESP_LOGD(TAG, "spi_mode %02x", phdr->spi_mode );
    ESP_LOGD(TAG, "spi_speed %02x", phdr->spi_speed );
    ESP_LOGD(TAG, "spi_size %02x", phdr->spi_size );

    const char* str;
    switch ( phdr->spi_speed ) {
    case ESP_IMAGE_SPI_SPEED_40M:
        str = "40MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_26M:
        str = "26.7MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_20M:
        str = "20MHz";
        break;
    case ESP_IMAGE_SPI_SPEED_80M:
        str = "80MHz";
        break;
    default:
        str = "20MHz";
        break;
    }
    ESP_LOGI(TAG, "SPI Speed      : %s", str );

    switch ( phdr->spi_mode ) {
    case ESP_IMAGE_SPI_MODE_QIO:
        str = "QIO";
        break;
    case ESP_IMAGE_SPI_MODE_QOUT:
        str = "QOUT";
        break;
    case ESP_IMAGE_SPI_MODE_DIO:
        str = "DIO";
        break;
    case ESP_IMAGE_SPI_MODE_DOUT:
        str = "DOUT";
        break;
    default:
        str = "QIO";
        break;
    }
    ESP_LOGI(TAG, "SPI Mode       : %s", str );

    switch ( phdr->spi_size ) {
    case ESP_IMAGE_FLASH_SIZE_1MB:
        str = "1MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_2MB:
    case ESP_IMAGE_FLASH_SIZE_2MB_C1:
        str = "2MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_4MB:
    case ESP_IMAGE_FLASH_SIZE_4MB_C1:
        str = "4MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_8MB:
        str = "8MB";
        break;
    case ESP_IMAGE_FLASH_SIZE_16MB:
        str = "16MB";
        break;
    default:
        str = "2MB";
        break;
    }
    ESP_LOGI(TAG, "SPI Flash Size : %s", str );
#endif
}
int main()
{
    /* Set system clock */
    ccu_sys(clock_sys);
    
    /* Set cpu clock*/
//     ccu_cpu(clock_cpu);
    
    /* init memory control unit */
    sys_dram_init(clock_dram);

    /* init uart */
    uart_init();
    pr_init();
    
//     
//     boot_norflash();
//     
//     if (check_device())
//     {
//         uart_printf("[preload]: booting from device \n");
//         
//         uart_printf("[preload] cannot find system\n");
//     }
//     
    pr_boot("return FEL\n");
}
