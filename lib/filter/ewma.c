#include <filter/ewma.h>
#include <mm.h>
#include <types.h>
#include <stddef.h>
#include <state.h>
#include <math.h>

ewma_t *ewma_alloc(float weight)
{
    ewma_t *ewma;
    ewma = kmalloc(sizeof(ewma_t), GFP_KERNEL);
    if(ewma == NULL)
        return NULL;

    ewma->last = NAN;
    ewma->weight = weight;
    return ewma;
}

state ewma_free(ewma_t *ewma)
{
    if(ewma == NULL)
        return -EINVAL;
    kfree(ewma);
    return -ENOERR;
}   

float ewma_filter(ewma_t *ewma, float val)
{
    if(isnan(ewma->last))
        ewma->last = val;
    else
        ewma->last = (ewma->weight * val)+((1-ewma->weight) * ewma->last);
    return ewma->last;
}

state ewma_clear(ewma_t *ewma)
{
    if(!ewma)
        return -EINVAL;
    ewma->last = NAN;
    return -ENOERR;
}
