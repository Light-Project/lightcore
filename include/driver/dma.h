/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DMA_H_
#define _DRIVER_DMA_H_

#include <types.h>

struct dma_request {
    struct list_head list;
    unsigned int index;
    dma_addr_t dest, src;
    size_t len;
};

struct dma_channel {

};

struct dma_device {
    struct dma_ops *ops;
    struct list_head request;
};

struct dma_ops {
    state (*memcpy)(struct dma_device *,  );
};

#endif  /* _DRIVER_DMA_H_ */
