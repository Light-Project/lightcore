#include <printk.h>
#include <kfifo.h> 

#define ringbuffer_len (1<<12)
static char print_ringbuffer[ringbuffer_len];

static struct kfifo ringbuffer = {
    .buffer = print_ringbuffer,
};

void prb_

