#ifndef _SYSTEM_MM_HEAP_H_
#define _SYSTEM_MM_HEAP_H_

#include <types.h>

typedef struct{
    
    /* point to the lowest free memory */
    uint32_t  used
    /* point to the lowest free memory */
    size_t    lfree
} heaphead_TypeDef;

typedef struct{
    uint32_t  protect;
    size_t    use;
    size_t    prev,next;
} heap_TypeDef;

#define heap_protect((uint32_t)0x55AA00FF) 


#endif
