/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 Sanpe <sanpeqf@gmail.com>
 */

static uint8_t dme1737_led_reg(struct led_device *ldev)
{
    struct dme1737_device *dme1737 = ldev->pdata;
    uint8_t which;

    if (&dme1737->led[0] == ldev)
        which = DME1737_RTM_LED0;
    else
        which = DME1737_RTM_LED1;

    return which;
}

static state dme1737_brightness_get(struct led_device *ldev, enum led_brightness *bn)
{
    struct dme1737_device *dme1737 = ldev->pdata;
    uint8_t value, reg = dme1737_led_reg(ldev);

    value = dme1737_rtm_read(dme1737, reg);
    if (value != DME1737_RTM_LED_OFF)
        *bn = LED_BRIGHTNESS_ON;
    else
        *bn = LED_BRIGHTNESS_OFF;

    return -ENOERR;
}

static state dme1737_brightness_set(struct led_device *ldev, enum led_brightness bn)
{
    struct dme1737_device *dme1737 = ldev->pdata;
    uint8_t value, reg = dme1737_led_reg(ldev);

    if (bn != LED_BRIGHTNESS_OFF)
        value = DME1737_RTM_LED_ON;
    else
        value = DME1737_RTM_LED_OFF;

    dme1737_rtm_write(dme1737, reg, value);
    return -ENOERR;
}

static state dme1737_blink_get(struct led_device *ldev, unsigned int *on, unsigned int *off)
{
    struct dme1737_device *dme1737 = ldev->pdata;
    uint8_t value, reg = dme1737_led_reg(ldev);

    value = dme1737_rtm_read(dme1737, reg);
    switch (value) {
        case DME1737_RTM_LED_OFF:
            *on = 0;
            *off = 1000;
            break;

        case DME1737_RTM_LED_BLANK0:
            *on = 500;
            *off = 500;
            break;

        case DME1737_RTM_LED_BLANK1:
            *on = 500;
            *off = 1500;
            break;

        case DME1737_RTM_LED_ON:
            *on = 1000;
            *off = 0;
            break;

        default:
            return -ENOTSUPP;
    }

    return -ENOERR;
}

static state dme1737_blink_set(struct led_device *ldev, unsigned int on, unsigned int off)
{
    struct dme1737_device *dme1737 = ldev->pdata;
    uint8_t value, reg = dme1737_led_reg(ldev);

    if (!on)
        value = DME1737_RTM_LED_OFF;
    else if (!off)
        value = DME1737_RTM_LED_ON;
    else if (on < off)
        value = DME1737_RTM_LED_BLANK1;
    else /* on >= off */
        value = DME1737_RTM_LED_BLANK0;

    dme1737_rtm_write(dme1737, reg, value);
    return -ENOERR;
}

static struct led_ops dme1737_led_ops = {
    .brightness_get = dme1737_brightness_get,
    .brightness_set = dme1737_brightness_set,
    .blink_get = dme1737_blink_get,
    .blink_set = dme1737_blink_set,
};

static state dme1737_led_probe(struct dme1737_device *dme1737)
{
    unsigned int count;
    state retval;

    for (count = 0; count < 2; ++count) {
        dme1737->led[count].dev = &dme1737->isa->dev;
        dme1737->led[count].ops = &dme1737_led_ops;
        dme1737->led[count].default_trigger = "heartbeat";
        dme1737->led[count].max_brightness = LED_BRIGHTNESS_ON;
        dme1737->led[count].pdata = dme1737;

        retval = led_register(&dme1737->led[count]);
        if (retval)
            break;
    }

    return retval;
}
