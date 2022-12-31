/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "dme1737"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <ioops.h>
#include <kmalloc.h>
#include <timekeeping.h>
#include <driver/isa.h>
#include <driver/hwmon.h>
#include <driver/hwmon/dme1737.h>

#define DME1737_DATA_INTERVAL ((uint64_t)NSEC_PER_SEC * 1)
#define DME1737_VBAT_INTERVAL ((uint64_t)NSEC_PER_SEC * 600)

/* ---------------------------------------------------------------------
 * Registers
 *
 * The sensors are defined as follows:
 *
 * Voltages                          Temperatures
 * --------                          ------------
 * in0   +5VTR (+5V stdby)           temp1   Remote diode 1
 * in1   Vccp  (proc core)           temp2   Internal temp
 * in2   VCC   (internal +3.3V)      temp3   Remote diode 2
 * in3   +5V
 * in4   +12V
 * in5   VTR   (+3.3V stby)
 * in6   Vbat
 * in7   Vtrip
 *
 * --------------------------------------------------------------------- */

static const char *const dme1737_in_label[] = {
    "+5VTR", "Vccp", "VCC", "+5V",
    "+12V", "VTR", "Vbat", "Vtrip",
};

static const char *const dme1737_temp_label[] = {
    "Remote diode 1", "Internal temp",
    "Remote diode 2",
};

static struct hwmon_channel_info dme1737_info[] = {
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_IN,
        HWMON_IN_HAS_LABEL | HWMON_IN_HAS_DATA | /* in0 */
        HWMON_IN_HAS_MIN   | HWMON_IN_HAS_MAX,
        HWMON_IN_HAS_LABEL | HWMON_IN_HAS_DATA | /* in1 */
        HWMON_IN_HAS_MIN   | HWMON_IN_HAS_MAX,
        HWMON_IN_HAS_LABEL | HWMON_IN_HAS_DATA | /* in2 */
        HWMON_IN_HAS_MIN   | HWMON_IN_HAS_MAX,
        HWMON_IN_HAS_LABEL | HWMON_IN_HAS_DATA | /* in3 */
        HWMON_IN_HAS_MIN   | HWMON_IN_HAS_MAX,
        HWMON_IN_HAS_LABEL | HWMON_IN_HAS_DATA | /* in4 */
        HWMON_IN_HAS_MIN   | HWMON_IN_HAS_MAX,
        HWMON_IN_HAS_LABEL | HWMON_IN_HAS_DATA | /* in5 */
        HWMON_IN_HAS_MIN   | HWMON_IN_HAS_MAX,
        HWMON_IN_HAS_LABEL | HWMON_IN_HAS_DATA | /* in6 */
        HWMON_IN_HAS_MIN   | HWMON_IN_HAS_MAX,
        HWMON_IN_HAS_LABEL | HWMON_IN_HAS_DATA | /* in7 */
        HWMON_IN_HAS_MIN   | HWMON_IN_HAS_MAX,
    ),
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_THERMAL,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA | /* temp0 */
        HWMON_THERMAL_HAS_MIN   | HWMON_THERMAL_HAS_MAX,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA | /* temp1 */
        HWMON_THERMAL_HAS_MIN   | HWMON_THERMAL_HAS_MAX,
        HWMON_THERMAL_HAS_LABEL | HWMON_THERMAL_HAS_DATA | /* temp2 */
        HWMON_THERMAL_HAS_MIN   | HWMON_THERMAL_HAS_MAX,
    ),
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_FAN,
        HWMON_FAN_HAS_DATA | HWMON_FAN_HAS_TARGET | /* fan0 */
        HWMON_FAN_HAS_MIN,
        HWMON_FAN_HAS_DATA | HWMON_FAN_HAS_TARGET | /* fan1 */
        HWMON_FAN_HAS_MIN,
        HWMON_FAN_HAS_DATA | HWMON_FAN_HAS_TARGET | /* fan2 */
        HWMON_FAN_HAS_MIN,
        HWMON_FAN_HAS_DATA | HWMON_FAN_HAS_TARGET | /* fan3 */
        HWMON_FAN_HAS_MIN,
        HWMON_FAN_HAS_DATA | HWMON_FAN_HAS_TARGET | /* fan4 */
        HWMON_FAN_HAS_MIN  | HWMON_FAN_HAS_MAX,
        HWMON_FAN_HAS_DATA | HWMON_FAN_HAS_TARGET | /* fan6 */
        HWMON_FAN_HAS_MIN  | HWMON_FAN_HAS_MAX,
    ),
    HWMON_CHANNEL_INFO(
        HWMON_SENSOR_PWM,
        HWMON_PWM_HAS_DATA | HWMON_PWM_HAS_FREQ | /* pwm0 */
        HWMON_PWM_HAS_MIN  | HWMON_PWM_HAS_MODE |
        HWMON_PWM_HAS_RAMP,
        HWMON_PWM_HAS_DATA | HWMON_PWM_HAS_FREQ | /* pwm1 */
        HWMON_PWM_HAS_MIN  | HWMON_PWM_HAS_MODE |
        HWMON_PWM_HAS_RAMP,
        HWMON_PWM_HAS_DATA | HWMON_PWM_HAS_FREQ | /* pwm2 */
        HWMON_PWM_HAS_MIN  | HWMON_PWM_HAS_MODE |
        HWMON_PWM_HAS_RAMP,
        HWMON_PWM_HAS_DATA | HWMON_PWM_HAS_FREQ | /* pwm3 */
        HWMON_PWM_HAS_MIN  | HWMON_PWM_HAS_MODE,
        HWMON_PWM_HAS_DATA | HWMON_PWM_HAS_FREQ | /* pwm4 */
        HWMON_PWM_HAS_MIN  | HWMON_PWM_HAS_MODE,
        HWMON_PWM_HAS_DATA | HWMON_PWM_HAS_FREQ | /* pwm5 */
        HWMON_PWM_HAS_MIN  | HWMON_PWM_HAS_MODE,
    ),
    { }, /* NULL */
};

static const unsigned int dme1737_reg_in_lsb[] = {
    3, 4, 4, 3, 2, 0, 0, 5
};

static const unsigned int dme1737_reg_in_lsb_shl[] = {
    4, 4, 0, 0, 0, 0, 4, 4
};

static const unsigned int dme1737_reg_temp_lsb[] = {
    1, 2, 1
};

static const unsigned int dme1737_reg_temp_lsb_shl[] = {
    4, 4, 0
};

static const unsigned int sch311x_in_nominal[8] = {
    2500, 1500, 3300, 5000,
    12000, 3300, 3300
};

static const unsigned int sch51xx_in_nominal[8] = {
    2500, 2250, 3300, 1125,
    1125, 3300, 3300, 1500
};

struct dme1737_device {
    struct hwmon_device hwmon;
    struct isa_device *isa;
    struct mutex lock;

    resource_size_t sio;
    resource_size_t runtime;

    bool valid;
    ktime_t last_data;
    ktime_t last_vbat;
    const unsigned int *in_nominal;

    struct {
        uint16_t data;
        uint8_t min;
        uint8_t max;
    } in[8];

    struct {
        int16_t data;
        uint8_t min;
        uint8_t max;
        uint8_t offset;
    } temp[3];

    struct {
        uint16_t data;
        uint16_t min;
        uint8_t max;
        uint8_t opt;
    } fan[6];

    struct {
        uint8_t data;
        uint8_t min;
        uint8_t config;
        uint8_t acz;
        uint8_t freq;
        uint8_t rr;
    } pwm[6];
};

#define hwmon_to_dme1737(ptr) \
    container_of(ptr, struct dme1737_device, hwmon)

static __always_inline void
dme1737_sio_entry(resource_size_t sio)
{
    outb(DME1737_ADDR(sio), DME1737_SIO_ENTRY);
}

static __always_inline void
dme1737_sio_exit(resource_size_t sio)
{
    outb(DME1737_ADDR(sio), DME1737_SIO_EXIT);
}

static __always_inline uint8_t
dme1737_index_read(resource_size_t base, uint8_t index)
{
    outb(DME1737_ADDR(base), index);
    return inb(DME1737_DATA(base));
}

static __always_inline void
dme1737_index_write(resource_size_t base, uint8_t index, uint8_t data)
{
    outb(DME1737_ADDR(base), index);
    outb(DME1737_DATA(base), data);
}

static __always_inline uint8_t
dme1737_hwm_read(struct dme1737_device *dme1737, uint8_t index)
{
    resource_size_t base = dme1737->runtime + DME1737_RTM_HWM_ADDR;
    return dme1737_index_read(base, index);
}

static __always_inline void
dme1737_hwm_write(struct dme1737_device *dme1737, uint8_t index, uint8_t data)
{
    resource_size_t base = dme1737->runtime + DME1737_RTM_HWM_ADDR;
    dme1737_index_write(base, index, data);
}

static __always_inline int
dme1737_regin(int reg, unsigned int nominal, unsigned int res)
{
    return (reg * nominal + (3 << (res - 3))) / (3 << (res - 2));
}

static __always_inline int
dme1737_inreg(int val, unsigned int nominal)
{
    clamp_adj(val, 0, 255 * nominal / 192);
    return DIV_ROUND_CLOSEST(val * 192, nominal);
}

static __always_inline int
dme1737_regtemp(int reg, unsigned int res)
{
	return (reg * 1000) >> (res - 8);
}

static __always_inline int
dme1737_tempreg(int val, unsigned int nominal)
{
	clamp_adj(val, -128000, 127000);
	return DIV_ROUND_CLOSEST(val, 1000);
}

static void dme1737_hwm_update(struct dme1737_device *dme1737)
{
    ktime_t currtime = timekeeping_get_time();
    uint8_t value, lsb[6];
    unsigned int count;

    mutex_lock(&dme1737->lock);

    if (ktime_after(currtime, dme1737->last_vbat + DME1737_VBAT_INTERVAL) || !dme1737->valid) {
        value = dme1737_hwm_read(dme1737, DME1737_HWM_CONFIG);
        value |= DME1737_HWM_CONFIG_VBAT_MON;
        dme1737_hwm_write(dme1737, DME1737_HWM_CONFIG, value);
        dme1737->last_vbat = currtime;
    }

    if (ktime_before(currtime, dme1737->last_data + DME1737_DATA_INTERVAL) && dme1737->valid)
        goto finish;

    for (count = 0; count < ARRAY_SIZE(dme1737->in); ++count) {
        dme1737->in[count].data = dme1737_hwm_read(dme1737, DME1737_HWM_VIN(count)) << 8;
        dme1737->in[count].min = dme1737_hwm_read(dme1737, DME1737_HWM_VMIN(count));
        dme1737->in[count].max = dme1737_hwm_read(dme1737, DME1737_HWM_VMAX(count));
    }

    for (count = 0; count < ARRAY_SIZE(dme1737->temp); ++count) {
        dme1737->temp[count].data = dme1737_hwm_read(dme1737, DME1737_HWM_TEMP(count)) << 8;
        dme1737->temp[count].min = dme1737_hwm_read(dme1737, DME1737_HWM_TMIN(count));
        dme1737->temp[count].max = dme1737_hwm_read(dme1737, DME1737_HWM_TMAX(count));
    }

    for (count = 0; count < ARRAY_SIZE(lsb); ++count)
        lsb[count] = dme1737_hwm_read(dme1737, DME1737_HWM_ADLSB(count));

    for (count = 0; count < ARRAY_SIZE(dme1737->in); ++count) {
        dme1737->in[count].data |=
            (lsb[dme1737_reg_in_lsb[count]] << dme1737_reg_in_lsb_shl[count]) & 0xf0;
    }

    for (count = 0; count < ARRAY_SIZE(dme1737->temp); ++count) {
        dme1737->temp[count].data |=
            (lsb[dme1737_reg_temp_lsb[count]] << dme1737_reg_temp_lsb_shl[count]) & 0xf0;
    }

    dme1737->last_data = currtime;
    dme1737->valid = true;

finish:
    mutex_unlock(&dme1737->lock);
}

static state dme1737_string(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                            unsigned int index, uint32_t att, const char **str)
{
    switch (sensor) {
        case HWMON_SENSOR_IN:
            switch (att) {
                case HWMON_IN_LABEL:
                    *str = dme1737_in_label[index];
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        case HWMON_SENSOR_THERMAL:
            switch (att) {
                case HWMON_THERMAL_LABEL:
                    *str = dme1737_temp_label[index];
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

static state dme1737_read(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                          unsigned int index, uint32_t att, ssize_t *val)
{
    struct dme1737_device *dme1737 = hwmon_to_dme1737(hdev);
    int value;

    dme1737_hwm_update(dme1737);

    switch (sensor) {
        case HWMON_SENSOR_IN:
            switch (att) {
                case HWMON_IN_DATA:
                    value = dme1737_regin(dme1737->in[index].data,
                                          dme1737->in_nominal[index], 16);
                    *val = (ssize_t)value;
                    break;

                case HWMON_IN_MIN:
                    value = dme1737_regin(dme1737->in[index].min,
                                          dme1737->in_nominal[index], 8);
                    *val = (ssize_t)value;
                    break;

                case HWMON_IN_MAX:
                    value = dme1737_regin(dme1737->in[index].max,
                                          dme1737->in_nominal[index], 8);
                    *val = (ssize_t)value;
                    break;

                default:
                    return -EOPNOTSUPP;
            }
            break;

        case HWMON_SENSOR_THERMAL:
            switch (att) {
                case HWMON_THERMAL_DATA:
                    value = dme1737_regtemp(dme1737->temp[index].data, 16);
                    *val = (ssize_t)value;
                    break;

                case HWMON_THERMAL_MIN:
                    value = dme1737_regtemp(dme1737->temp[index].min, 8);
                    *val = (ssize_t)value;
                    break;

                case HWMON_THERMAL_MAX:
                    value = dme1737_regtemp(dme1737->temp[index].max, 8);
                    *val = (ssize_t)value;
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

static state dme1737_write(struct hwmon_device *hdev, enum hwmon_sensor sensor,
                           unsigned int index, uint32_t att, ssize_t val)
{
    return -ENOERR;
}

static struct hwmon_ops dme1737_ops = {
    .string = dme1737_string,
    .read = dme1737_read,
    .write = dme1737_write,
};

static state dme1737_probe(struct isa_device *idev, unsigned int index, const void *pdata)
{
    struct dme1737_device *dme1737 = (void *)pdata;

    dme1737->hwmon.dev = &idev->dev;
    dme1737->hwmon.ops = &dme1737_ops;
    dme1737->hwmon.info = dme1737_info;
    isa_set_devdata(idev, dme1737);

    return hwmon_register(&dme1737->hwmon);
}

static state dme1737_detect(struct dme1737_device *dme1737, resource_size_t sio)
{
    state retval = -ENOERR;
    uint8_t value;
    const char *name;

    /* Unlock the chip register */
    dme1737_sio_entry(sio);

    /* Get chip device id */
    value = dme1737_index_read(sio, DME1737_SIO_DEVID);

    switch (value) {
        case SCH3112_ID:
            name = "SCH3112";
            goto sch311x;

        case SCH3114_ID:
            name = "SCH3114";
            goto sch311x;

        case SCH3116_ID:
            name = "SCH3116";
            goto sch311x;

        sch311x:
            dme1737->in_nominal = sch311x_in_nominal;
            break;

        case SCH5127_ID:
            name = "SCH5127";
            goto sch51xx;

        case SCH5147_ID:
            name = "SCH5147";
            goto sch51xx;

        sch51xx:
            dme1737->in_nominal = sch51xx_in_nominal;
            break;

        default:
            retval = -ENODEV;
            goto finish;
    }

    value = dme1737_index_read(sio, DME1737_SIO_DEVREV);
    isa_info(dme1737->isa, "chipset %s.%02x detect\n", name, value);

    /* Select logical device A (runtime registers) */
    dme1737_index_write(sio, DME1737_SIO_LOGICAL, 0x0a);

    /* Get the base address of the runtime registers */
    dme1737->runtime = dme1737_index_read(sio, DME1737_SIO_BASE_MSB) << 8;
    dme1737->runtime |= dme1737_index_read(sio, DME1737_SIO_BASE_LSB);
    dme1737->sio = sio;

    if (!dme1737->runtime) {
        isa_err(dme1737->isa, "runtime base not set\n");
        retval = -ENODATA;
    }

finish:
    dme1737_sio_exit(sio);
    return retval;
}

static state dme1737_match(struct isa_device *idev, unsigned int index)
{
    struct dme1737_device *dme1737;
    state retval;

    dme1737 = kzalloc(sizeof(*dme1737), GFP_KERNEL);
    if (unlikely(!dme1737))
        return -ENOMEM;

    dme1737->isa = idev;
    if ((retval = dme1737_detect(dme1737, DME1737_SIO_BASE0)) &&
        (retval = dme1737_detect(dme1737, DME1737_SIO_BASE1)) &&
        (retval = dme1737_detect(dme1737, DME1737_SIO_BASE2)) &&
        (retval = dme1737_detect(dme1737, DME1737_SIO_BASE3))) {
        kfree(dme1737);
        return retval;
    }

    isa_set_devdata(idev, dme1737);
    return -ENOERR;
}

static struct isa_driver dme1737_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .match = dme1737_match,
    .probe = dme1737_probe,
};

static state dme1737_init(void)
{
    return isa_driver_register(&dme1737_driver, 1, NULL);
}
driver_initcall(dme1737_init);
