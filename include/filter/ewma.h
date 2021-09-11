
#ifndef _EWMA_H_
#define _EWMA_H_
 
#include <types.h>
#include <stddef.h>
#include <state.h>
#include <math.h>

typedef struct ewma{
    float last;
    float weight;
} ewma_t;


ewma_t *ewma_alloc(float weight);
state ewma_free(ewma_t *ewma);
float ewma_filter(ewma_t *ewma, float val);
state ewma_clear(ewma_t *ewma);

#endif
