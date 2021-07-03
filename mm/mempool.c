#include <stddef.h>
#include <state.h>
#include <mm.h>
#include <mm/mempool.h>

#ifdef CONFIG_MEMPOOL_DEBUG

#else

#endif

state mempool_init_node(mempool_t *pool, int nr_min, mempool_alloc_t alloc_fn,
                    mempool_free_t *free_fn, void *pool_data, 
                    gfp_t gfp_mask, int node_id)
{

    spin_lock_init(&pool->lock);

    pool->min_nr    = nr_min;
    pool->pool_data = pool_data;
    pool->alloc     = alloc_fn;
    pool->free      = free_fn;
    pool->elements  = kcalloc(nr_min, MSIZE);

    if(!pool->elements)
        return -ENOMEM;
    
    /*
     * First pre-allocate the guaranteed number of buffers.
     */
    void *element;
    for(pool->curr_nr = 0; pool->curr_nr < pool->min_nr;)
    {
        element = pool->alloc(gfp_mask, pool->pool_data);
        if(unlikely(!element)){

            return -ENOMEM;
        }

    }
    return -ENOERR;
}

struct mempool mempool_init(mempool_t *pool, int min, mempool_alloc_t *alloc_fn,
                    mempool_free_t *free_fn, void *data)
{
    pool->

}


struct mempool mempool_create(int min, mempool_alloc_t *alloc_fn,
                    mempool_free_t *free_fn, void *data)
{
    mempool_t *pool;
    
    pool = kzalloc(sizeof(mempool_t));


}

mempool_alloc()
{


}


mempool_free(mempool_t *pool, void *mem)
{
    if(unlikely(pool&&mem))
        return;

    pool->free(mem, pool->pool_data);
}
