/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <chip/esp8266/iomux.h>
#include <driver/i2s/i2s-esp8266.h>
#include <asm/io.h>
#include <stdlib.h>

#define I2S_BASE_CLK 104000000
#define esp8266_i2s_base ((void *)0x60000e00)

static void esp8266_i2s_config(struct i2s_port *port)
{
    uint8_t bck_div = 1;
    uint8_t mclk_div = 1;
    uint32_t val;

    // Calculate the frequency division corresponding to the bit rate
    uint32_t scaled_base_freq = I2S_BASE_CLK / 32;
    float delta_best = scaled_base_freq;

    for (uint8_t i = 1; i < 64; i++) {
        for (uint8_t j = i; j < 64; j++) {
            float new_delta = abs(((float)scaled_base_freq / i / j) - rate);

            if (new_delta < delta_best) {
                delta_best = new_delta;
                bck_div = i;
                mclk_div = j;
            }
        }
    }

    val = readl(esp8266_i2s_base + I2S_ESP8266_CONF);
    val &= ~I2S_ESP8266_CONF_BCK_DIV;
    val |= (bck_div << 22) & I2S_ESP8266_CONF_BCK_DIV;
    writel(esp8266_i2s_base + I2S_ESP8266_CONF, val);

    val = readl(esp8266_i2s_base + I2S_ESP8266_CONF);
    val &= ~I2S_ESP8266_CONF_CLKM_DIV;
    val |= (mclk_div << 16) & I2S_ESP8266_CONF_CLKM_DIV;
    writel(esp8266_i2s_base + I2S_ESP8266_CONF, val);
}

static void esp8266_i2s_write(const char *src, unsigned int len)
{
    while(len--)
        writel(esp8266_i2s_base + I2S_ESP8266_TXFIFO, *src++);
}

static void esp8266_i2s_startup()
{
    uint32_t val;

    ESP8266_IOMUX2->FUNL = 1;
    ESP8266_IOMUX2->FUNH = 0;
    ESP8266_IOMUX3->FUNL = 1;
    ESP8266_IOMUX3->FUNH = 0;
    ESP8266_IOMUX15->FUNL = 1;
    ESP8266_IOMUX15->FUNH = 0;

    /* Reset i2s */
    writel(esp8266_i2s_base + I2S_ESP8266_CONF, I2S_ESP8266_CONF_RESET_MASK);
    writel(esp8266_i2s_base + I2S_ESP8266_CONF, 0);

    esp8266_i2s_rate(160000);

    val = readl(esp8266_i2s_base + I2S_ESP8266_CONF);
    val |= 8 << 12;
    val |= I2S_ESP8266_CONF_TRANS_MSB_SHIFT | I2S_ESP8266_CONF_TX_START;
    writel(esp8266_i2s_base + I2S_ESP8266_CONF, val);

}

static struct i2s_ops esp8266_i2s_ops = {
    .startup = esp8266_i2s_startup,
    .shutdown = ,
    .config = ,
    .write = ,
};

