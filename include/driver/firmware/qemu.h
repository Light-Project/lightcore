#ifndef _DRIVER_FIRMWARE_QEMU_H_
#define _DRIVER_FIRMWARE_QEMU_H_

#include <types.h>

struct qemu_dma_access {
    be32 control;
    be32 length;
    be64 address;
} __packed;

#define QEMU_DMA_CTRL_ERROR     0x01
#define QEMU_DMA_CTRL_READ      0x02
#define QEMU_DMA_CTRL_SKIP      0x04
#define QEMU_DMA_CTRL_SELECT    0x08
#define QEMU_DMA_CTRL_WRITE     0x10

#endif  /* _DRIVER_FIRMWARE_QEMU_H_ */
