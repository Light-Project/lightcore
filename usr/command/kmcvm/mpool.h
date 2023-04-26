#ifndef _LOCAL_MPOOL_H_
#define _LOCAL_MPOOL_H_

#ifndef __MPOOL__H__
#define __MPOOL__H__

#include <stdarg.h>
#include <stddef.h>
#include <memory.h>

typedef struct mpool_pool_s            mpool_t;
typedef struct mpool_log_s             mpool_log_t;
typedef int                            mpool_fd_t;

struct mpool_log_s {
};

extern unsigned  mpool_pagesize;

#define mpool_align_ptr(p, a)                                                   \
    (char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))


#define  MPOOL_ALIGNMENT   sizeof(unsigned long)    /* platform word */

#define  MPOOL_OK          0
#define  MPOOL_ERROR      -1
#define  MPOOL_AGAIN      -2
#define  MPOOL_BUSY       -3
#define  MPOOL_DONE       -4
#define  MPOOL_DECLINED   -5
#define  MPOOL_ABORT      -6


#define mpool_memzero(buf, n)       (void) memset(buf, 0, n)


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (mpool_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define NGX_MAX_ALLOC_FROM_POOL  (mpool_pagesize - 1)

#define MPOOL_DEFAULT_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16
#define NGX_MIN_POOL_SIZE                                                     \
    ngx_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)),            \
              NGX_POOL_ALIGNMENT)


typedef void (*mpool_cleanup_pt)(void *data);

typedef struct mpool_cleanup_s  mpool_cleanup_t;

struct mpool_cleanup_s {
    mpool_cleanup_pt   handler;
    void                 *data;
    mpool_cleanup_t   *next;
};


typedef struct ngx_pool_large_s  ngx_pool_large_t;

struct ngx_pool_large_s {
    ngx_pool_large_t     *next;
    void                 *alloc;
};


typedef struct {
    char               *last;
    char               *end;
    mpool_t           *next;
    unsigned            failed;
} ngx_pool_data_t;


struct mpool_pool_s {
    ngx_pool_data_t       d;
    size_t                max;
    mpool_t           *current;
    ngx_pool_large_t     *large;
    mpool_cleanup_t   *cleanup;
    mpool_log_t            *log;
};


typedef struct {
    mpool_fd_t              fd;
    char                  *name;
    mpool_log_t            *log;
} ngx_pool_cleanup_file_t;


mpool_t *mpool_create(size_t size, mpool_log_t *log);
void mpool_destroy(mpool_t *pool);
void mpool_reset(mpool_t *pool);

void *mpool_alloc(mpool_t *pool, size_t size);
void *mpool_nalloc(mpool_t *pool, size_t size);
void *mpool_calloc(mpool_t *pool, size_t size);
void *mpool_pmemalign(mpool_t *pool, size_t size, size_t alignment);
int  mpool_free(mpool_t *pool, void *p);

mpool_cleanup_t *mpool_cleanup_add(mpool_t *p, size_t size);


#endif  /*__MPOOL__H__*/



#endif /* _LOCAL_MPOOL_H_ */
