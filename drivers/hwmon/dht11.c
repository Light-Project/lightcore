/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "dht11"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <mutex.h>
#include <delay.h>
#include <timekeeping.h>
#include <driver/platform.h>
#include <driver/gpio.h>
#include <driver/hwmon.h>

#define DHT11_START_HOLD        20
#define DHT11_DATA_THRESHOLD    45000
#define DHT11_DATA_TIMEOUT      400000
#define DHT11_DATA_INTERVAL     2000000000

#define DHT11_THERMAL_MIN      -20000
#define DHT11_THERMAL_MAX       60000
#define DHT11_HUMIDITY_MIN      5000
#define DHT11_HUMIDITY_MAX      95000

union dht11_packet {
    le64 value;
    uint8_t bytes[5];
    struct {
        uint8_t checksum;
        uint8_t thermal_low:7;
        uint8_t thermal_neg:1;
        uint8_t thermal_high;
        uint8_t humidity_low;
        uint8_t humidity_high;
    };
} __packed;

struct dht11_device {
    struct hwmon_device hwmon;
    struct gpio_channel *bus;
    union dht11_packet last_data;
    ktime_t last_time;
    struct mutex lock;
};

#define hwmon_to_dht11(ptr) \
    container_of(ptr, struct dht11_device, hwmon)

static struct hwmon_channel_info dht11_hwmon_info[] = {
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_THERMAL,
        HWMON_THERMAL_HAS_DATA |
        HWMON_THERMAL_HAS_MIN | HWMON_THERMAL_HAS_MAX,
    ),
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_HUMIDITY,
        HWMON_THERMAL_HAS_DATA |
        HWMON_THERMAL_HAS_MIN | HWMON_THERMAL_HAS_MAX,
    ),
    { }, /* NULL */
};

static uint8_t dht11_checksum(const union dht11_packet *packet)
{
    const uint8_t *bytes = packet->bytes + 1;
    return bytes[0] + bytes[1] + bytes[2] + bytes[3];
}

static state dht11_wait(struct dht11_device *dht11, ktime_t *delta, bool value)
{
    ktime_t start = timekeeping_get_time();
    state retval;
    bool level;

    while (!(retval = gpioc_value_get(dht11->bus, &level)) && level != value) {
        if (ktime_after(timekeeping_get_time(), start + DHT11_DATA_TIMEOUT))
            return -ETIMEDOUT;
        cpu_relax();
    }

    if (!retval && delta)
        *delta = ktime_sub(timekeeping_get_time(), start);

    return retval;
}

static state dht11_tranfer(struct dht11_device *dht11, uint64_t *value)
{
    unsigned int index;
    ktime_t delta;
    state retval;

    /* Send start signal */
    retval = gpioc_value_set(dht11->bus, false);
    if (retval)
        return retval;
    msleep(DHT11_START_HOLD);
    retval = gpioc_value_set(dht11->bus, true);
    if (retval)
        return retval;

    /* Wait for device response */
    if ((retval = dht11_wait(dht11, NULL, false)) ||
        (retval = dht11_wait(dht11, NULL, true))  ||
        (retval = dht11_wait(dht11, NULL, false))) {
        dev_warn(dht11->hwmon.dev, "device response error %d\n", retval);
        return retval;
    }

    /* Read 40bit sensor data */
    for (*value = 0, index = 40; index; --index) {
        if ((retval = dht11_wait(dht11, NULL, true)) ||
            (retval = dht11_wait(dht11, &delta, false))) {
            dev_warn(dht11->hwmon.dev, "read data error %d\n", retval);
            return retval;
        }
        if (ktime_to_ns(delta) > DHT11_DATA_THRESHOLD)
            *value |= BIT_ULL(index - 1);
    }

    return -ENOERR;
}

static state dht11_data(struct dht11_device *dht11, union dht11_packet *packet)
{
    ktime_t currtime = timekeeping_get_time();
    unsigned int count;
    uint64_t value;
    state retval;

    if (ktime_before(currtime, dht11->last_time + DHT11_DATA_INTERVAL)) {
        *packet = dht11->last_data;
        return -ENOERR;
    }

    mutex_lock(&dht11->lock);
    for (count = 0; count < 3; ++count) {
        preempt_disable();
        retval = dht11_tranfer(dht11, &value);
        preempt_enable();

        if (retval != -ETIMEDOUT)
            break;
    }

    if (retval) {
        mutex_unlock(&dht11->lock);
        dev_err(dht11->hwmon.dev, "give up after three attempts\n");
        return retval;
    }

    dev_debug(dht11->hwmon.dev, "tranfer data %#llx\n", value);
    packet->value = cpu_to_le64(value);

    if (dht11_checksum(packet) != packet->checksum) {
        mutex_unlock(&dht11->lock);
        dev_err(dht11->hwmon.dev, "data checksum error %#04x -> %#04x\n",
                packet->checksum, dht11_checksum(packet));
        return -EPROTO;
    }

    dht11->last_time = timekeeping_get_time();
    dht11->last_data = *packet;
    mutex_unlock(&dht11->lock);

    return -ENOERR;
}

static state dht11_read(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                        unsigned int index, uint32_t att, ssize_t *val)
{
    struct dht11_device *dht11 = hwmon_to_dht11(hdev);
    union dht11_packet packet = {};
    unsigned int scrape;
    state retval;

    switch (sensor) {
        case HWMON_SENSOR_THERMAL:
            switch (att) {
                case HWMON_THERMAL_DATA:
                    retval = dht11_data(dht11, &packet);
                    if (retval)
                        return retval;

                    scrape = packet.thermal_high * 1000 +
                             packet.thermal_low * 100;
                    *val = packet.thermal_neg ? -scrape : scrape;
                    break;

                case HWMON_THERMAL_MIN:
                    *val = DHT11_THERMAL_MIN;
                    break;

                case HWMON_THERMAL_MAX:
                    *val = DHT11_THERMAL_MAX;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        case HWMON_SENSOR_HUMIDITY:
            switch (att) {
                case HWMON_HUMIDITY_DATA:
                    retval = dht11_data(dht11, &packet);
                    if (retval)
                        return retval;

                    scrape = packet.humidity_high * 1000 +
                             packet.humidity_low * 100;
                    *val = scrape;
                    break;

                case HWMON_HUMIDITY_MIN:
                    *val = DHT11_HUMIDITY_MIN;
                    break;

                case HWMON_HUMIDITY_MAX:
                    *val = DHT11_HUMIDITY_MAX;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        default:
            return -EOPNOTSUPP;
    }

    return -ENOERR;
}

static struct hwmon_ops dht11_hwmon_ops = {
    .read = dht11_read,
};

static state dht11_probe(struct platform_device *pdev, const void *pdata)
{
    struct dht11_device *dht11;

    dht11 = platform_kzalloc(pdev, sizeof(*dht11), GFP_KERNEL);
    if (unlikely(!dht11))
        return -ENOMEM;

    dht11->bus = gpio_fwnode_channel(pdev->dev.fwnode, NULL, 0);
    if (IS_INVAL(dht11->bus))
        return PTR_INVAL(dht11->bus);

    dht11->hwmon.dev = &pdev->dev;
    dht11->hwmon.ops = &dht11_hwmon_ops;
    dht11->hwmon.info = dht11_hwmon_info;
    platform_set_devdata(pdev, dht11);

    return hwmon_register(&dht11->hwmon);
}

static const struct dt_device_id dt11_ids[] = {
    { .compatible = "asair,dht11" },
    { }, /* NULL */
};

static struct platform_driver dht11_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = dt11_ids,
    .probe = dht11_probe,
};

static state dht11_init(void)
{
    return platform_driver_register(&dht11_driver);
}
driver_initcall_sync(dht11_init);
