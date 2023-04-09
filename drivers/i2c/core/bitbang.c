/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "i2c-bitbang"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <delay.h>
#include <bitrev.h>
#include <ticktime.h>
#include <driver/i2c-bitbang.h>
#include <export.h>

#define getsda(adap) (adap)->getsda((adap)->data)
#define getscl(adap) (adap)->getscl((adap)->data)
#define setsda(adap, val) (adap)->setsda((adap)->data, val)
#define setscl(adap, val) (adap)->setscl((adap)->data, val)

static __always_inline void sdalo(struct i2c_bitbang *adap)
{
    setsda(adap, false);
    udelay(DIV_ROUND_CLOSEST(adap->delay, 2));
}

static __always_inline void sdahi(struct i2c_bitbang *adap)
{
    setsda(adap, true);
    udelay(DIV_ROUND_CLOSEST(adap->delay, 2));
}

static __always_inline void scllo(struct i2c_bitbang *adap)
{
    setscl(adap, false);
    udelay(DIV_ROUND_CLOSEST(adap->delay, 2));
}

static state sclhi(struct i2c_bitbang *adap)
{
    ttime_t start;

    setscl(adap, true);
    if (!adap->getscl)
        goto finish;

    start = ticktime;
    while (!getscl(adap)) {
        if (ttime_after(ticktime, start + adap->timeout)) {
            if (getscl(adap))
                break;
            return -ETIMEDOUT;
        }
        cpu_relax();
    }

finish:
    udelay(adap->delay);
    return -ENOERR;
}

/* assert: scl high, sda high */
static void bitbang_start(struct i2c_bitbang *adap)
{
    setsda(adap, false);
    udelay(adap->delay);
    scllo(adap);
}

/* assert: scl low */
static void bitbang_restart(struct i2c_bitbang *adap)
{
    sdahi(adap);
    sclhi(adap);
    bitbang_start(adap);
}

/* assert: scl low */
static void bitbang_stop(struct i2c_bitbang *adap)
{
    sdalo(adap);
    sclhi(adap);
    setsda(adap, true);
    udelay(adap->delay);
}

/* assert: sda high, scl low */
static state bitbang_recv_ack(struct i2c_host *host)
{
    struct i2c_bitbang *adap = host->host_data;
    bool is_ack;

    if (sclhi(adap) < 0) {
        dev_err(host->dev, "recv-ack: timeout at ack\n");
        return -ETIMEDOUT;
    }

    /*
     * SDA should be pulled down by slave,
     * or it may NAK.
     */
    is_ack = !getsda(adap);
    scllo(adap);

    return is_ack ? -ENOERR : -ECONNABORTED;
}

/* assert: sda high, scl low */
static state bitbang_send_ack(struct i2c_host *host, bool is_ack)
{
    struct i2c_bitbang *adap = host->host_data;

    if (is_ack)
        setsda(adap, false);

    udelay(DIV_ROUND_CLOSEST(adap->delay, 2));
    if (sclhi(adap) < 0) {
        dev_err(host->dev, "send-ack: ack/nak timeout\n");
        return -ETIMEDOUT;
    }

    scllo(adap);
    return -ENOERR;
}

/* assert: scl low */
static state bitbang_recv_byte(struct i2c_host *host, struct i2c_transfer *trans,
                               uint8_t *value)
{
    struct i2c_bitbang *adap = host->host_data;
    unsigned int count;
    uint8_t data = 0;

    sdahi(adap);
    for (count = 0; count < BITS_PER_U8; ++count) {
        if (sclhi(adap)) {
            dev_err(host->dev, "send-byte: timeout at bit %d\n", count);
            return -ETIMEDOUT;
        }

        data <<= 1;
        if (getsda(adap))
            data |= 1;

        setscl(adap, false);
        udelay(count == BITS_PER_U8 - 1 ?
               DIV_ROUND_CLOSEST(adap->delay, 2) : adap->delay);
    }

    if (trans->flags & I2C_M_LSB)
        *value = bitrev8(data);
    else
        *value = data;

    return -ENOERR;
}

/* assert: scl low */
static state bitbang_send_byte(struct i2c_host *host, struct i2c_transfer *trans,
                               uint8_t value)
{
    struct i2c_bitbang *adap = host->host_data;
    unsigned int count;
    uint8_t data;

    if (trans->flags & I2C_M_LSB)
        data = bitrev8(value);
    else
        data = value;

    for (count = 0; count < BITS_PER_U8; ++count) {
        setsda(adap, data & 0x80);
        data <<= 1;
        udelay(DIV_ROUND_CLOSEST(adap->delay, 2));

        if (sclhi(adap)) {
            dev_err(host->dev, "send-byte: %#04x timeout at bit %d\n", data, count);
            return -ETIMEDOUT;
        }

        scllo(adap);
    }

    sdahi(adap);
    return -ENOERR;
}

static state bitbang_try_addr(struct i2c_host *host, struct i2c_transfer *trans,
                              uint8_t addr, unsigned int retries)
{
    struct i2c_bitbang *adap = host->host_data;
    unsigned int count;
    state retval = -ENOERR;

    for (count = 0; count <= retries; count++) {
        retval = bitbang_send_byte(host, trans, addr);
        if (retval)
            return retval;

        retval = bitbang_recv_ack(host);
        if (!retval || count == retries)
            break;

        bitbang_stop(adap);
        udelay(adap->delay);
        bitbang_start(adap);
    }

    return retval;
}

static state bitbang_recv_bytes(struct i2c_host *host, struct i2c_transfer *trans)
{
    uint8_t *buff = trans->buf;
    unsigned int count = trans->len;
    state retval;

    for (count = 0; count < trans->len; ++count, ++buff) {
        retval = bitbang_recv_byte(host, trans, buff);
        if (retval)
            return retval;

        if (!trans->len && trans->flags & I2C_M_RECV_LEN) {
            if (*buff <= 0 || *buff > SMBUS_BLOCK_MAX) {
                dev_err(host->dev, "recv-bytes: invalid block length %d\n", *buff);
                return -EPROTO;
            }
            trans->len += *buff;
        }

        if (!(trans->flags & I2C_M_NO_RD_ACK)) {
            retval = bitbang_send_ack(host, count != trans->len - 1);
            if (retval)
                return retval;
        }
    }

    return -ENOERR;
}

static state bitbang_send_bytes(struct i2c_host *host, struct i2c_transfer *trans)
{
    const uint8_t *buff = trans->buf;
    unsigned int count;
    state retval;

    for (count = 0; count < trans->len; ++count, ++buff) {
        retval = bitbang_send_byte(host, trans, *buff);
        if (retval) {
            dev_err(host->dev, "send-bytes: error %d\n", retval);
            return retval;
        }

        retval = bitbang_recv_ack(host);
        if (retval == -ECONNABORTED) {
            if (!(trans->flags & I2C_M_IGNORE_NAK)) {
                dev_err(host->dev, "send-bytes: recvack nak bailout\n");
                return retval;
            }
        } else if (retval) {
            dev_err(host->dev, "send-bytes: recvack error %d\n", retval);
            return retval;
        }
    }

    return -ENOERR;
}

static state bitbang_send_addr(struct i2c_host *host, struct i2c_transfer *trans)
{
    struct i2c_bitbang *adap = host->host_data;
    unsigned int retries = 0;
    uint8_t address;
    state retval;

    if (!(trans->flags & I2C_M_IGNORE_NAK))
        retries = host->retries;

    if (!(trans->flags & I2C_M_TEN)) {
        /* 7bit normal address */
        address = i2c_transfer_addr8(trans);
        if (trans->flags & I2C_M_REV_DIR_ADDR)
            address ^= 1;
        /* try normal address */
        retval = bitbang_try_addr(host, trans, address, retries);
        if (retval && (retval != -ECONNABORTED ||
            !(trans->flags & I2C_M_IGNORE_NAK))) {
            dev_err(host->dev, "send-addr: try normal address error\n");
            return retval;
        }
    } else {
        /* 10bit extended address */
        address = trans->addr >> 7;
        /* try extended address */
        retval = bitbang_try_addr(host, trans, address, retries);
        if (retval && (retval != -ECONNABORTED ||
            !(trans->flags & I2C_M_IGNORE_NAK))) {
            dev_err(host->dev, "send-addr: try extended address error\n");
            return retval;
        }
        /* send normal address */
        retval = bitbang_send_byte(host, trans, trans->addr);
        if (retval && (retval != -ECONNABORTED ||
            !(trans->flags & I2C_M_IGNORE_NAK))) {
            dev_err(host->dev, "send-addr: try remaining address error\n");
            return retval;
        }
        /* switch read direction */
        if (trans->flags & I2C_M_RD) {
            bitbang_restart(adap);
            address |= 1;
            retval = bitbang_send_byte(host, trans, address);
            if (retval && (retval != -ECONNABORTED ||
                !(trans->flags & I2C_M_IGNORE_NAK))) {
                dev_err(host->dev, "send-addr: try repeated address error\n");
                return retval;
            }
        }
    }

    return -ENOERR;
}

static state bitbang_transfer(struct i2c_host *host, unsigned int num,
                              struct i2c_transfer *trans)
{
    struct i2c_bitbang *adap = host->host_data;
    struct i2c_transfer *xfer;
    unsigned int count;
    state retval = -ENOERR;

    if (adap->prepare) {
        retval = adap->prepare(adap->data);
        if (retval)
            return retval;
    }

    bitbang_start(adap);
    for (count = 0; count < num; ++count) {
        xfer = trans + count;

        if (!(xfer->flags & I2C_M_NOSTART)) {
            if (count) {
                if (!((xfer - 1)->flags & I2C_M_STOP))
                    bitbang_restart(adap);
                else {
                    bitbang_stop(adap);
                    bitbang_start(adap);
                }
            }
            retval = bitbang_send_addr(host, xfer);
            if (retval)
                goto finish;
        }

        if (xfer->flags & I2C_M_RD) {
            retval = bitbang_recv_bytes(host, xfer);
            if (retval)
                return retval;
        } else {
            retval = bitbang_send_bytes(host, xfer);
            if (retval)
                return retval;
        }
    }

finish:
    bitbang_stop(adap);
    if (adap->finish)
        adap->finish(adap->data);

    return retval;
}

static struct i2c_ops bitbang_ops = {
    .i2c_transfer = bitbang_transfer,
};

state i2c_bitbang_register(struct i2c_host *host)
{
    struct i2c_bitbang *adap = host->host_data;

    if (!adap->setscl || !adap->setsda || !adap->getsda)
        return -EINVAL;

    if (!adap->getscl) {
        dev_warn(host->dev, "not i2c compliant: can't read scl\n");
        dev_warn(host->dev, "bus may be unreliable\n");
    }

    host->retries = 3;
    host->ops = &bitbang_ops;

    return i2c_host_register(host);
}
EXPORT_SYMBOL(i2c_bitbang_register);
