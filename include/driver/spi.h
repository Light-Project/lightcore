/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_SPI_H_
#define _DRIVER_SPI_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>
#include <devtable.h>

enum spi_device_mode {
    SPI_CPHA    = BIT(0),
    SPI_CPOL    = BIT(1),

    SPI_MODE0   = 0,
    SPI_MODE1   = SPI_CPHA,
    SPI_MODE2   = SPI_CPOL,
    SPI_MODE3   = SPI_CPOL | SPI_CPHA,

    SPI_LSBF    = BIT(2),   /* First transmit bit select */
    SPI_CS_HI   = BIT(3),   /* High level enable */
};

enum spi_transfer_mode {
    SPI_TRAN_SINGLE  = 0,   /* Single line transfer */
    SPI_TRAN_DUAL    = 1,   /*  Dual  line transfer */
    SPI_TRAN_QUAD    = 2,   /*  Quad  line transfer */
};

struct spi_device {
    struct device device;
    struct dt_node *dt_node;
    struct spi_host *host;
    enum spi_device_mode mode;
    unsigned int dev_nr;
};

struct spi_driver {
    struct driver driver;
    struct dt_device_id *dt_table;
    state (*probe)(struct spi_device *);
    void (*remove)(struct spi_device *);
	void (*shutdown)(struct spi_device *spi);
};

struct spi_transfer {
    unsigned int len;
    void *tx_buf;
    void *rx_buf;

    uint8_t dummy:1;
    uint8_t tx_lines:2;
    uint8_t rx_lines:2;
    uint32_t freq;

    struct list_head list;
};

struct spi_request {
    struct spi_device *spi_device;
    struct list_head list;
    struct list_head transfer;
};

struct spi_host {
    struct dt_node *dt_node;

    /* limit frequency for host */
    uint32_t max_freq;
    uint32_t min_freq;

    unsigned int cs_num;
    enum spi_device_mode mode;

    struct spi_ops *ops;
};

struct spi_ops {
    state (*startup)(struct spi_host *);
    void  (*shutdown)(struct spi_host *);
    state (*transfer)(struct spi_host *, struct spi_device *, struct spi_transfer *);
    state (*enqueue)(struct spi_host *, struct spi_request *);
    state (*devsel)(struct spi_host *, struct spi_device *, bool);
};

state spi_driver_register(struct spi_driver *);
void spi_driver_unregister(struct spi_driver *);

state spi_host_register(struct spi_host *);
void spi_host_unregister(struct spi_host *);

#endif  /* _DRIVER_SPI_H_ */
