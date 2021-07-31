/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_DMA_H_
#define _DRIVER_DMA_H_ 

#include <types.h>

struct dma_ops;

struct dma_chan {

};

struct dma_device {
    struct dma_ops *ops;
};

struct dma_ops {
    state (*memcpy)(struct dma_chan *chan, dma_addr_t dest, 
                    dma_addr_t src, size_t len);
    
};

#endif  /* _DRIVER_DMA_H_ */
