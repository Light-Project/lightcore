/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "suniv-spi"
#define pr_fmt(fmt) DRIVER_NAME fmt

#include <mm.h>
#include <initcall.h>
#include <irq.h>
#include <driver/platform.h>
#include <driver/reset.h>
#include <driver/clk.h>
#include <driver/irqchip.h>
#include <driver/spi.h>
#include <driver/spi/suniv.h>

#include <asm/io.h>

struct suniv_device {
    struct spi_host spi;
    struct clk_channel *clk;
    struct irqchip_channel *irqchip;
    struct reset_channel *reset;
    void *base;

    /* fifo pointer */
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    unsigned int len;
};

#define spi_to_suniv(host) \
    container_of(host, struct suniv_device, spi);

static inline uint32_t suniv_read(struct suniv_device *sdev, int reg)
{
    return readl(sdev->base + reg);
}

static inline void suniv_write(struct suniv_device *sdev, int reg, uint32_t val)
{
    writel(sdev->base + reg, val);
}

static void suniv_spi_fifo_fill(struct suniv_device *sdev)
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

static void suniv_spi_fifo_drain(struct suniv_device *sdev)
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

static irqreturn_t suniv_spi_handler(irqnr_t vector, void *data)
{
    struct suniv_device *sdev = data;
    uint32_t val;

    val = suniv_read(sdev, SUNIV_SPI_ISR);

    if (val & SUNIV_SPI_INT_TF_EMP) {
        suniv_spi_fifo_drain(sdev);
        suniv_write(sdev, SUNIV_SPI_ISR, SUNIV_SPI_INT_TF_ERQ);
        return IRQ_RET_HANDLED;
    }

    if (val & SUNIV_SPI_INT_TF_ERQ) {
        suniv_spi_fifo_fill(sdev);
        suniv_write(sdev, SUNIV_SPI_ISR, SUNIV_SPI_INT_TF_ERQ);
        return IRQ_RET_HANDLED;
    }

    if (val & SUNIV_SPI_INT_RF_RDY) {
        suniv_spi_fifo_drain(sdev);
        suniv_write(sdev, SUNIV_SPI_ISR, SUNIV_SPI_INT_RF_RDY);
        return IRQ_RET_HANDLED;
    }

    return IRQ_RET_NONE;
}

static state suniv_spi_transmit(struct spi_host *host, struct spi_request *req)
{
    struct suniv_device *sdev = spi_to_suniv(host);
    struct spi_device *spi = req->spi_device;
    uint32_t val;

    sdev->tx_buf = req->tx_buf;
    sdev->rx_buf = req->rx_buf;
    sdev->len = req->len;

    /* Clear all interrupts */
    suniv_write(sdev, SUNIV_SPI_ISR, ~0U);

    /* Reset Fifos */
    val = suniv_read(sdev, SUNIV_SPI_FCR);
    val |= SUNIV_SPI_FCR_TX_FIFO_RST | SUNIV_SPI_FCR_RX_FIFO_RST;
    suniv_write(sdev, SUNIV_SPI_FCR, val);

    /* Setup the transfer mode */
    val = suniv_read(sdev, SUNIV_SPI_TCR);

    if (spi->mode & SPI_CPHA)
        val |= SUNIV_SPI_TCR_CPHA;
    else
        val &= ~SUNIV_SPI_TCR_CPHA;
    if (spi->mode & SPI_CPOL)
        val |= SUNIV_SPI_TCR_CPOL;
    else
        val &= ~SUNIV_SPI_TCR_CPOL;
    if (spi->mode & SPI_LSBF)
        val |= SUNIV_SPI_TCR_FBS;
    else
        val &= ~SUNIV_SPI_TCR_FBS;

    if (!req->rx_buf)
        val |= SUNIV_SPI_TCR_DHB;
    else
        val &= ~SUNIV_SPI_TCR_DHB;

    suniv_write(sdev, SUNIV_SPI_TCR, val);

    val = SUNIV_SPI_INT_TF_EMP;
    if (sdev->len > SUNIV_SPI_FIFO_SIZE)
        val |= SUNIV_SPI_INT_TF_ERQ | SUNIV_SPI_INT_RF_RDY;
    suniv_write(sdev, SUNIV_SPI_IER, val);

    suniv_spi_fifo_fill(sdev);

	/* Start the transfer */
    val = suniv_read(sdev, SUNIV_SPI_TCR);
    suniv_write(sdev, SUNIV_SPI_TCR, val | SUNIV_SPI_TCR_XCH);

    /* Mask all interrupts */
    suniv_write(sdev, SUNIV_SPI_IER, 0);
    return -ENOERR;
}

static state suniv_spi_devsel(struct spi_host *host, struct spi_device *dev, bool sel)
{
    struct suniv_device *sdev = spi_to_suniv(host);
    uint32_t val;

    val = suniv_read(sdev, SUNIV_SPI_TCR);
    val &= ~SUNIV_SPI_TCR_SS_SEL;
    val |= (dev->dev_nr << 4) & SUNIV_SPI_TCR_SS_SEL;

    if (sel && (dev->mode & SPI_CS_HI))
        val |= SUNIV_SPI_TCR_SS_VAL;
    else if (sel && !(dev->mode & SPI_CS_HI))
        val &= SUNIV_SPI_TCR_SS_VAL;
    else if (!sel && (dev->mode & SPI_CS_HI))
        val &= SUNIV_SPI_TCR_SS_VAL;
    else
        val |= SUNIV_SPI_TCR_SS_VAL;

    suniv_write(sdev, SUNIV_SPI_TCR, val);
}

static struct spi_ops suniv_spi_ops = {
    .transfer = suniv_spi_transmit,
    .devsel = suniv_spi_devsel,
};

static state suniv_spi_probe(struct platform_device *pdev, const void *pdata)
{
    struct suniv_device *sdev;
    resource_size_t addr, size;
    uint32_t val;

    sdev = dev_kzalloc(&pdev->dev, sizeof(*sdev), GFP_KERNEL);
    if (!sdev)
        return -ENOMEM;

    val = platform_resource_type(pdev, 0);
    if (val != RESOURCE_MMIO)
        return -ENODEV;

    addr = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    sdev->base = dev_ioremap(&pdev->dev, addr, size);
    if (!sdev->base)
        return -ENOMEM;

    /* request irq handle */
    irq_request(0, 0, suniv_spi_handler, sdev, DRIVER_NAME);

    sdev->clk = dt_get_clk_channel(pdev->dt_node, 0);
    if (sdev->irqchip) {
        dev_err(&pdev->dev, "unable to request clock\n");
        return -ENODEV;
    }
    reset_reset(sdev->reset);

    sdev->reset = dt_get_reset_channel(pdev->dt_node, 0);
    if (sdev->irqchip) {
        dev_err(&pdev->dev, "unable to request reset\n");
        return -ENODEV;
    }
    reset_reset(sdev->reset);

    sdev->irqchip = dt_get_irqchip_channel(pdev->dt_node, 0);
    if (sdev->irqchip) {
        dev_err(&pdev->dev, "unable to request irqchip\n");
        return -ENODEV;
    }
    irqchip_pass(sdev->irqchip);

    /* fill host info */
    sdev->spi.min_freq = 3000;
    sdev->spi.max_freq = 100000000;
    sdev->spi.cs_num = 4;
    sdev->spi.mode = SPI_CPHA | SPI_CPOL | SPI_LSBF | SPI_CS_HI;
    sdev->spi.dt_node = pdev->dt_node;

    sdev->spi.ops = &suniv_spi_ops;
    return spi_host_register(&sdev->spi);
}

static struct dt_device_id suniv_ids[] = {
    { .compatible = "allwinner,suniv-spi" },
    { }, /* NULL */
};

static struct platform_driver suniv_spi_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = suniv_ids,
    .probe = suniv_spi_probe,
};

static state suniv_spi_init(void)
{
    return platform_driver_register(&suniv_spi_driver);
}
driver_initcall(suniv_spi_init);
