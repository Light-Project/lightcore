/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_SPI_H_
#define _DRIVER_SPI_H_

#include <types.h>
#include <state.h>
#include <list.h>
#include <device.h>
#include <resource.h>

enum spi_device_mode {
    SPI_CPHA    = BIT(0),
    SPI_CPOL    = BIT(1),

    SPI_MODE0   = 0,
    SPI_MODE1   = SPI_CPHA,
    SPI_MODE2   = SPI_CPOL,
    SPI_MODE3   = SPI_CPOL | SPI_CPHA,

    SPI_LSBF    = BIT(2),
    SPI_CS_HI   = BIT(3),
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
    state (*remove)(struct spi_device *);
};

struct spi_request {
    void *tx_buf;
    void *rx_buf;
    unsigned int len;

    uint8_t dummy:1;
    struct spi_device *spi_device;

    struct list_head list;
};

struct spi_host {
    struct spi_ops *ops;
};

struct spi_ops {
    state (*transfer)(struct spi_host *, struct spi_request *);
    void (*cs_set)(struct spi_host *, struct spi_device *, bool);
};

state spi_host_register(struct spi_host *);
state spi_host_unregister(struct spi_host *);

#endif  /* _DRIVER_SPI_H_ */
