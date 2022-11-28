/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "i2c-bitbang"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <delay.h>
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

/* assert: scl high, sda high  */
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
static state bitbang_recv_ack(struct i2c_bitbang *adap)
{
    bool is_ack;

    if (sclhi(adap) < 0)
        return -ETIMEDOUT;

    is_ack = !getsda(adap);
    scllo(adap);

    return is_ack ? -ENOERR : -ECONNABORTED;
}

/* assert: sda high, scl low */
static state bitbang_send_ack(struct i2c_bitbang *adap, bool is_ack)
{
    if (is_ack)
        setsda(adap, false);

    udelay(DIV_ROUND_CLOSEST(adap->delay, 2));
    if (sclhi(adap) < 0)
        return -ETIMEDOUT;

    scllo(adap);
    return -ENOERR;
}

/* assert: scl low */
static state bitbang_recv_byte(struct i2c_bitbang *adap, uint8_t *value)
{
    unsigned int count;
    uint8_t indata = 0;

    sdahi(adap);
    for (count = 0; count < BITS_PER_U8; ++count) {
        if (sclhi(adap))
            return -ETIMEDOUT;

        indata <<= 1;
        if (getsda(adap))
            indata |= 1;

        setscl(adap, false);
        udelay(count == BITS_PER_U8 - 1 ?
               DIV_ROUND_CLOSEST(adap->delay, 2) : adap->delay);
    }

    *value = indata;
    return -ENOERR;
}

/* assert: scl low */
static state bitbang_send_byte(struct i2c_bitbang *adap, uint8_t value)
{
    unsigned int count;

    for (count = 0; count < BITS_PER_U8; ++count) {
        setsda(adap, value & 0x80);
        value <<= 1;
        udelay(DIV_ROUND_CLOSEST(adap->delay, 2));

        if (sclhi(adap))
            return -ETIMEDOUT;

        scllo(adap);
    }

    sdahi(adap);
    return -ENOERR;
}

static state bitbang_try_addr(struct i2c_bitbang *adap, uint8_t addr,
                              unsigned retries)
{
    unsigned int count;
    state retval = -ENOERR;

    for (count = 0; count <= retries; count++) {
        retval = bitbang_send_byte(adap, addr);
        if (retval)
            return retval;

        retval = bitbang_recv_ack(adap);
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
    struct i2c_bitbang *adap = host->host_data;
    uint8_t *buff = trans->buf;
    unsigned int count = trans->len;
    state retval;

    for (count = 0; count < trans->len; ++count, ++buff) {
        retval = bitbang_recv_byte(adap, buff);
        if (retval) {
            return retval;
        }

        if (!trans->len && trans->flags & I2C_M_RECV_LEN) {
            if (*buff <= 0 || *buff > SMBUS_BLOCK_MAX) {
                dev_err(host->dev, "recvbytes: invalid block length %d\n", *buff);
                return -EPROTO;
            }
            trans->len += *buff;
        }

        if (!(trans->flags & I2C_M_NO_RD_ACK)) {
            retval = bitbang_send_ack(adap, count != trans->len - 1);
            if (retval) {
                return retval;
            }
        }
    }

    return -ENOERR;
}

static state bitbang_send_bytes(struct i2c_host *host, struct i2c_transfer *trans)
{
    struct i2c_bitbang *adap = host->host_data;
    const uint8_t *buff = trans->buf;
    unsigned int count;
    state retval;

    for (count = 0; count < trans->len; ++count, ++buff) {
        retval = bitbang_send_byte(adap, *buff);
        if (retval) {
            dev_err(host->dev, "sendbyte error %d\n", retval);
            return retval;
        }

        retval = bitbang_recv_ack(adap);
        if (retval == -ECONNABORTED) {
            if (!(trans->flags & I2C_M_IGNORE_NAK)) {
                dev_err(host->dev, "recvack nak bailout\n");
                return retval;
            }
        } else if (retval) {
            dev_err(host->dev, "recvack error %d\n", retval);
            return retval;
        }
    }

    return -ENOERR;
}

static state bitbang_send_addr(struct i2c_host *host, struct i2c_transfer *trans)
{
    struct i2c_bitbang *adap = host->host_data;
    uint8_t address;
    state retval;

    if (!(trans->flags & I2C_M_TEN)) {
        address = i2c_transfer_addr8(trans);
        if (trans->flags & I2C_M_REV_DIR_ADDR)
            address ^= 1;
        retval = bitbang_try_addr(adap, address, host->retries);
        if (retval && (retval != -ECONNABORTED ||
            !(trans->flags & I2C_M_IGNORE_NAK)))
            return retval;
    } else {


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
