#ifndef _CHIP_ESP8266_GPIO_H_
#define _CHIP_ESP8266_GPIO_H_

#include <types.h>

typedef union {
    struct {
        uint32_t sleep_oe:      1;
        uint32_t sleep_sel:     1;
        uint32_t reserved1:     1;
        uint32_t sleep_pullup:  1;
        uint32_t func_low_bit:  2;
        uint32_t reserved2:     1;
        uint32_t pullup:        1;
        uint32_t func_high_bit: 1;
    };
    struct {
        uint32_t func_low_bit:  2;
        uint32_t reserved1:     1;
        uint32_t pulldown:      1;
        uint32_t reserved2:     1;
        uint32_t sleep_pulldown: 1;
        uint32_t func_high_bit: 1;
    } rtc_pin;
    uint32_t val;
} gpio_pin_reg_t;

typedef struct{
    uint32_t out;
    uint32_t out_w1ts;
    uint32_t out_w1tc;

    uint32_t enable;
    uint32_t enable_w1ts;
    uint32_t enable_w1tc;

    uint32_t in;

    uint32_t status;
    uint32_t status_w1ts;
    uint32_t status_w1tc;
    
    struct{
            uint32_t source:        1;
            uint32_t reserved1:     1;
            uint32_t driver:        1;
            uint32_t reserved2:     4;
            uint32_t int_type:      3;
            uint32_t wakeup_enable: 1;
            uint32_t reserved3:     21;
    } pin[16];

    uint32_t sigma_delta;

    uint32_t rtc_calib_sync;
    uint32_t rtc_calib_value;
} __packed gpio_struct_t;

#define ESP8266_GPIO_BASE   ((uint32_t)0x60000300)

#define ESP8266_GPIO    ((gpio_struct_t*) ESP8266_GPIO_BASE)

#endif
