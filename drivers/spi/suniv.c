/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "suniv-spi"

#include <mm.h>
#include <initcall.h>
#include <irq.h>
#include <driver/platform.h>
#include <driver/spi.h>
#include <driver/spi/suniv.h>

#include <asm/io.h>

struct suniv_device {
    struct spi_host spi;
    void *mmio;

    uint8_t *tx_buf;
    uint8_t *rx_buf;
    unsigned int len;
};

#define spi_to_suniv(host) \
    container_of(host, struct suniv_device, spi);

static inline uint32_t suniv_read(struct suniv_device *sdev, int reg)
{
    return readl(sdev->mmio + reg);
}

static inline void suniv_write(struct suniv_device *sdev, int reg, uint32_t val)
{
    writel(sdev->mmio + reg, val);
}

static void suniv_fifo_fill(struct suniv_device *sdev)
{
    uint32_t count;
    uint8_t data;

    count = suniv_read(sdev, SUNIV_SPI_FCR);
    count = (count >> 16) & SUNIV_SPI_FSR_TF_CNT;
    count = min(count, sdev->len);

    while (count--) {
        data = sdev->tx_buf ? *sdev->tx_buf++ : 0;
        suniv_write(sdev, SUNIV_SPI_TXD, data);
    }

    sdev->len -= count;
}

static void suniv_fifo_drain(struct suniv_device *sdev)
{
    uint32_t count;
    uint8_t data;

    count = suniv_read(sdev, SUNIV_SPI_FCR);
    count = SUNIV_SPI_FSR_RF_CNT;

    while (count--) {
        data = suniv_read(sdev, SUNIV_SPI_RXD);
        if (sdev->rx_buf)
            *sdev->rx_buf++ = data;
    }
}

static irqreturn_t suniv_handler(irqnr_t vector, void *data)
{
    struct suniv_device *sdev = data;
    uint32_t val;

    val = suniv_read(sdev, SUNIV_SPI_ISR);

    if (val & SUNIV_SPI_INT_TF_EMP) {
        suniv_fifo_drain(sdev);
        suniv_write(sdev, SUNIV_SPI_ISR, SUNIV_SPI_INT_TF_ERQ);
        return IRQ_RET_HANDLED;
    }

    if (val & SUNIV_SPI_INT_TF_ERQ) {
        suniv_fifo_fill(sdev);
        suniv_write(sdev, SUNIV_SPI_ISR, SUNIV_SPI_INT_TF_ERQ);
        return IRQ_RET_HANDLED;
    }

    if (val & SUNIV_SPI_INT_RF_RDY) {
        suniv_fifo_drain(sdev);
        suniv_write(sdev, SUNIV_SPI_ISR, SUNIV_SPI_INT_RF_RDY);
        return IRQ_RET_HANDLED;
    }

    return IRQ_RET_NONE;
}

static void suniv_mode_set(struct suniv_device *sdev, enum spi_device_mode mode)
{
    uint32_t val;

    val = suniv_read(sdev, SUNIV_SPI_TCR);
    if (mode & SPI_CPHA)
        val |= SUNIV_SPI_TCR_CPHA;
    else
        val &= ~SUNIV_SPI_TCR_CPHA;
    if (mode & SPI_CPOL)
        val |= SUNIV_SPI_TCR_CPOL;
    else
        val &= ~SUNIV_SPI_TCR_CPOL;
    if (mode & SPI_LSBF)
        val |= SUNIV_SPI_TCR_FBS;
    else
        val &= ~SUNIV_SPI_TCR_FBS;
    suniv_write(sdev, SUNIV_SPI_TCR, val);
}

static state suniv_transmit(struct spi_host *host, struct spi_request *req)
{
    struct suniv_device *sdev = spi_to_suniv(host);
    uint32_t val;

    sdev->tx_buf = req->tx_buf;
    sdev->rx_buf = req->rx_buf;
    sdev->len = req->len;

    /* Clear all interrupts */
    suniv_write(sdev, SUNIV_SPI_ISR, ~0U);

    /* Reset fifo */
    val = suniv_read(sdev, SUNIV_SPI_FCR);
    val |= SUNIV_SPI_FCR_TX_FIFO_RST | SUNIV_SPI_FCR_RX_FIFO_RST;
    suniv_write(sdev, SUNIV_SPI_FCR, val);

    suniv_mode_set(sdev, req->spi_device->mode);

    val = SUNIV_SPI_INT_TF_EMP;
    if (sdev->len > SUNIV_SPI_FIFO_SIZE)
        val |= SUNIV_SPI_INT_TF_ERQ | SUNIV_SPI_INT_RF_RDY;
    suniv_write(sdev, SUNIV_SPI_IER, val);

    suniv_fifo_fill(sdev);

	/* Start the transfer */
    val = suniv_read(sdev, SUNIV_SPI_TCR);
    suniv_write(sdev, SUNIV_SPI_TCR, val | SUNIV_SPI_TCR_XCH);

    suniv_write(sdev, SUNIV_SPI_IER, 0);
    return -ENOERR;
}

static void suniv_cs_set(struct spi_host *host, struct spi_device *dev, bool enable)
{
    struct suniv_device *sdev = spi_to_suniv(host);
    uint32_t val;

    val = suniv_read(sdev, SUNIV_SPI_TCR);
    val &= ~SUNIV_SPI_TCR_SS_SEL;
    val |= (dev->dev_nr << 4) & SUNIV_SPI_TCR_SS_SEL;

    if (enable && (dev->mode & SPI_CS_HI))
        val |= SUNIV_SPI_TCR_SS_VAL;
    else if (enable && !(dev->mode & SPI_CS_HI))
        val &= SUNIV_SPI_TCR_SS_VAL;
    else if (!enable && (dev->mode & SPI_CS_HI))
        val &= SUNIV_SPI_TCR_SS_VAL;
    else
        val |= SUNIV_SPI_TCR_SS_VAL;

    suniv_write(sdev, SUNIV_SPI_TCR, val);
}

static struct spi_ops suniv_ops = {
    .transfer = suniv_transmit,
    .cs_set = suniv_cs_set,
};

static state suniv_probe(struct platform_device *pdev)
{
    struct suniv_device *sdev;
    resource_size_t addr, size;
    uint32_t val;

    sdev = kzalloc(sizeof(*sdev), GFP_KERNEL);
    if (!sdev)
        return -ENOMEM;

    val = platform_resource_type(pdev, 0);
    if (val != RESOURCE_MMIO)
        return -ENODEV;

    addr = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    sdev->mmio = dev_ioremap(&pdev->device, addr, size);
    if (!sdev->mmio)
        return -ENOMEM;

    irq_request(0, 0, suniv_handler, sdev, DRIVER_NAME);

    sdev->spi.ops = &suniv_ops;
    return spi_host_register(&sdev->spi);
}

static void suniv_remove(struct platform_device *pdev)
{

}

static struct dt_device_id suniv_ids[] = {
    { .compatible = "allwinner,suniv-spi" },
    { }, /* NULL */
};

static struct platform_driver suniv_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = suniv_ids,
    .probe = suniv_probe,
    .remove = suniv_remove,
};

static state suniv_init(void)
{
    return platform_driver_register(&suniv_driver);
}
driver_initcall(suniv_init);
