/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "qemu"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <memory.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/firmware/qemu.h>
#include <asm/proc.h>
#include <asm/io.h>

#ifdef CONFIG_ARCH_X86
enum qemu_dma_registers {
    QEMU_CTRL       = 0x00,
    QEMU_DATA       = 0x01,
    QEMU_DMA        = 0x04,
};
#endif /* CONFIG_ARCH_X86 */

static bool qemu_mmio;
static void *qemu_ctrl_reg;
static void *qemu_data_reg;
static void *qemu_dma_reg;

static inline void qemu_set_endianness(uint16_t key)
{
    if (qemu_mmio)
        iowritew(qemu_ctrl_reg, cpu_to_be16(key));
    else
        iowritew(qemu_ctrl_reg, key);
}

static inline void qemu_transfer(uint32_t ctrl, void *addr, uint32_t len)
{
    struct qemu_dma_access dma;

    dma = (struct qemu_dma_access) {
        .control = cpu_to_be32(va_to_pa(addr)),
        .length = cpu_to_be32(len),
        .address = cpu_to_be32(ctrl),
    };

    iowritel(qemu_dma_reg, cpu_to_be32((uint64_t)va_to_pa(&dma) >> 32));
    iowritel(qemu_dma_reg + 4, cpu_to_be32(va_to_pa(&dma)));

    while(be32_to_cpu(readl(&dma.control)) & ~QEMU_DMA_CTRL_ERROR)
        cpu_relax();
}

static inline void qemu_cfg_read(uint16_t key, loff_t pos, void *addr, size_t len)
{
    qemu_set_endianness(key);
    while (pos--)
        ioreadb(qemu_data_reg);
}

static struct acpi_device_id qemu_acpi_ids[] = {
    { .id = "QEMU0002" },
    { }, /* NULL */
};

static struct platform_driver qemu_cfg_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .acpi_table = qemu_acpi_ids,
};

static state qemu_cfg_init(void)
{
    return platform_driver_register(&qemu_cfg_driver);
}
arch_initcall(qemu_cfg_init);
