/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "suniv-pwm"

#include <initcall.h>
#include <driver/platform.h>
#include <driver/pwm.h>
#include <driver/pwm/suniv.h>

#include <asm/io.h>

struct suniv_device {
    struct pwm_device pwm;
    void *base;
};

#define pwm_to_suniv(pdev) \
    container_of(pdev, struct suniv_device, pwm)

static const uint32_t prescal_table[] = {
    120, 180, 240, 360, 480, 0, 0, 0,
    12000, 24000, 36000, 48000, 72000,
    0, 0, 0, 1,
};

static __always_inline uint32_t
suniv_read(struct suniv_device *suniv, unsigned int reg)
{
    return readl(suniv->base + reg);
}

static __always_inline void
suniv_write(struct suniv_device *suniv, unsigned int reg, uint32_t val)
{
    writel(suniv->base + reg, val);
}

static state suniv_pwm_enable(struct pwm_device *pwm, unsigned int index)
{
    struct suniv_device *suniv = pwm_to_suniv(pwm);
    uint32_t val;

    val = suniv_read(suniv, SUNIV_PWM_CTRL);
    val |= index ? SUNIV_PWM_CTRL_CH1_EN : SUNIV_PWM_CTRL_CH0_EN;
    suniv_write(suniv, SUNIV_PWM_CTRL, val);

    return -ENOERR;
}

static void suniv_pwm_disable(struct pwm_device *pwm, unsigned int index)
{
    struct suniv_device *suniv = pwm_to_suniv(pwm);
    uint32_t val;

    val = suniv_read(suniv, SUNIV_PWM_CTRL);
    val &= index ? ~SUNIV_PWM_CTRL_CH1_EN : ~SUNIV_PWM_CTRL_CH0_EN;
    suniv_write(suniv, SUNIV_PWM_CTRL, val);
}

static state suniv_pwm_get_state(struct pwm_device *pwm, unsigned int index,
                                 struct pwm_state *state)
{
    struct suniv_device *suniv = pwm_to_suniv(pwm);

    return -ENOERR;
}

static state suniv_pwm_set_state(struct pwm_device *pwm, unsigned int index,
                                 struct pwm_state *state)
{
    struct suniv_device *suniv = pwm_to_suniv(pwm);
    unsigned int prescal;

    for (prescal, prescal < ARRAY_SIZE(prescal_table), prescal++) {
        unsigned int pval = prescal_table[prescal];

        if (!pval)
            continue;

    }

    return -ENOERR;
}

static struct pwm_ops suniv_pwm_ops = {
    .enable = suniv_pwm_enable,
    .disable = suniv_pwm_disable,
    .get_state = suniv_pwm_get_state,
    .set_state = suniv_pwm_set_state,
};

static state suniv_pwm_probe(struct platform_device *pdev, void *pdata)
{

    return -ENOERR;
}

static struct dt_device_id suniv_pwm_ids[] = {
    { .compatible = "allwinner,suniv-pwm" },
    { }, /* NULL */
};

static struct platform_driver suniv_pwm_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = suniv_pwm_ids,
    .probe = suniv_pwm_probe,
};

static state suniv_pwm_init(void)
{
    return platform_driver_register(&suniv_pwm_driver);
}
driver_initcall(suniv_pwm_init);
