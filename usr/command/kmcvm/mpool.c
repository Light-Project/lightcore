#include "mpool.h"

#include <stdlib.h>
#include <kmalloc.h>

static inline void *mpool_alloc_small(mpool_t *pool, size_t size,
    unsigned align);
static void *mpool_alloc_block(mpool_t *pool, size_t size);
static void *mpool_alloc_large(mpool_t *pool, size_t size);


unsigned  mpool_pagesize = 4096;


void *
zhanghan_memalign(size_t alignment, size_t size, mpool_log_t *log)
{
    // void  *p;
    // int    err;

    // p =

    // if (err) {
    //     // ngx_log_error(NGX_LOG_EMERG, log, err,
    //     //               "posix_memalign(%uz, %uz) failed", alignment, size);
    //     p = NULL;
    // }

    // ngx_log_debug3(NGX_LOG_DEBUG_ALLOC, log, 0,
    //                "posix_memalign: %p:%uz @%uz", p, size, alignment);

    return kmalloc_align(size, GFP_KERNEL, alignment);
}


mpool_t *
mpool_create(size_t size, mpool_log_t *log)
{
    mpool_t  *p;

    p = zhanghan_memalign(NGX_POOL_ALIGNMENT, size, log);
    if (p == NULL) {
        return NULL;
    }

    p->d.last = (char *) p + sizeof(mpool_t);
    p->d.end = (char *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(mpool_t);
    p->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->large = NULL;
    p->cleanup = NULL;
    p->log = log;

    return p;
}


void
mpool_destroy(mpool_t *pool)
{
    mpool_t          *p, *n;
    ngx_pool_large_t    *l;
    mpool_cleanup_t  *c;

    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            // ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
            //                "run cleanup: %p", c);
            c->handler(c->data);
        }
    }

#if (NGX_DEBUG)

    /*
     * we could allocate the pool->log from this pool
     * so we cannot use this log while kfree()ing the pool
     */

    for (l = pool->large; l; l = l->next) {
        ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0, "kfree: %p", l->alloc);
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        ngx_log_debug2(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
                       "kfree: %p, unused: %uz", p, p->d.end - p->d.last);

        if (n == NULL) {
            break;
        }
    }

#endif

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            kfree(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        kfree(p);

        if (n == NULL) {
            break;
        }
    }
}


void
mpool_reset(mpool_t *pool)
{
    mpool_t        *p;
    ngx_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            kfree(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (char *) p + sizeof(mpool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    pool->large = NULL;
}


void *
mpool_alloc(mpool_t *pool, size_t size)
{
#if !(NGX_DEBUG_PALLOC)
    if (size <= pool->max) {
        return mpool_alloc_small(pool, size, 1);
    }
#endif

    return mpool_alloc_large(pool, size);
}


void *
mpool_nalloc(mpool_t *pool, size_t size)
{
#if !(NGX_DEBUG_PALLOC)
    if (size <= pool->max) {
        return mpool_alloc_small(pool, size, 0);
    }
#endif

    return mpool_alloc_large(pool, size);
}


static inline void *
mpool_alloc_small(mpool_t *pool, size_t size, unsigned align)
{
    char      *m;
    mpool_t  *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = mpool_align_ptr(m, MPOOL_ALIGNMENT);
        }

        if ((size_t) (p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }

        p = p->d.next;

    } while (p);

    return mpool_alloc_block(pool, size);
}


static void *
mpool_alloc_block(mpool_t *pool, size_t size)
{
    char      *m;
    size_t       psize;
    mpool_t  *p, *new;

    psize = (size_t) (pool->d.end - (char *) pool);

    m = zhanghan_memalign(NGX_POOL_ALIGNMENT, psize, pool->log);
    if (m == NULL) {
        return NULL;
    }

    new = (mpool_t *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(ngx_pool_data_t);
    m = mpool_align_ptr(m, MPOOL_ALIGNMENT);
    new->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;
        }
    }

    p->d.next = new;

    return m;
}


static void *
mpool_alloc_large(mpool_t *pool, size_t size)
{
    void              *p;
    unsigned          n;
    ngx_pool_large_t  *large;

    p = kmalloc(size, GFP_KERNEL);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = mpool_alloc_small(pool, sizeof(ngx_pool_large_t), 1);
    if (large == NULL) {
        kfree(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


void *
mpool_memalign(mpool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    ngx_pool_large_t  *large;

    p = zhanghan_memalign(alignment, size, pool->log);
    if (p == NULL) {
        return NULL;
    }

    large = mpool_alloc_small(pool, sizeof(ngx_pool_large_t), 1);
    if (large == NULL) {
        kfree(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


int
mpool_free(mpool_t *pool, void *p)
{
    ngx_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            // ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, pool->log, 0,
            //                "kfree: %p", l->alloc);
            kfree(l->alloc);
            l->alloc = NULL;

            return MPOOL_OK;
        }
    }

    return MPOOL_DECLINED;
}


void *
mpool_calloc(mpool_t *pool, size_t size)
{
    void *p;

    p = mpool_alloc(pool, size);
    if (p) {
        mpool_memzero(p, size);
    }

    return p;
}


mpool_cleanup_t *
mpool_cleanup_add(mpool_t *p, size_t size)
{
    mpool_cleanup_t  *c;

    c = mpool_alloc(p, sizeof(mpool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = mpool_alloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    // ngx_log_debug1(NGX_LOG_DEBUG_ALLOC, p->log, 0, "add cleanup: %p", c);

    return c;
}
