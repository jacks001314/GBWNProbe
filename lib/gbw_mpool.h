/*
 *
 *      Filename: gbw_mpool.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 10:07:10
 * Last Modified: 2017-01-09 10:07:10
 */

#ifndef GBW_MPOOL_H
#define GBW_MPOOL_H

typedef void (*gbw_pool_cleanup_pt)(void *data);
typedef struct gbw_pool_cleanup_t  gbw_pool_cleanup_t;
typedef struct gbw_pool_large_t gbw_pool_large_t;
typedef struct gbw_pool_t gbw_pool_t;
typedef struct gbw_pool_data_t gbw_pool_data_t;

#include "gbw_constants.h"
#include "gbw_list.h"

#define GBW_MAX_ALLOC_FROM_POOL  (4096 - 1)

#define GBW_DEFAULT_POOL_SIZE    (16 * 1024)

#define GBW_POOL_ALIGNMENT       16
#define GBW_MIN_POOL_SIZE                                                     \
    gbw_align((sizeof(gbw_pool_t) + 2 * sizeof(gbw_pool_large_t)),            \
              GBW_POOL_ALIGNMENT)



struct gbw_pool_cleanup_t {
    gbw_pool_cleanup_pt   handler;
    void                 *data;
    gbw_pool_cleanup_t   *next;
};


struct gbw_pool_large_t {
    gbw_pool_large_t     *next;
    void                 *alloc;
};


struct gbw_pool_data_t {
    void               *last;
    void               *end;
    gbw_pool_t           *next;
    size_t           failed;
};


struct gbw_pool_t {

    gbw_pool_data_t       d;
    
    struct list_head node;
    size_t                max;
    gbw_pool_t           *current;
    gbw_pool_large_t     *large;
    gbw_pool_cleanup_t   *cleanup;
};

extern gbw_pool_t *
gbw_pool_create(size_t size);

extern void 
gbw_pool_destroy(gbw_pool_t *pool);

extern void 
gbw_pool_reset(gbw_pool_t *pool);

extern void *
gbw_palloc(gbw_pool_t *pool, size_t size);

extern void *
gbw_pnalloc(gbw_pool_t *pool, size_t size);

extern void *
gbw_pcalloc(gbw_pool_t *pool, size_t size);

extern void *
gbw_pmemalign(gbw_pool_t *pool, size_t size, size_t alignment);

extern int 
gbw_pfree(gbw_pool_t *pool, void *p);


extern gbw_pool_cleanup_t *
gbw_pool_cleanup_add(gbw_pool_t *p, size_t size);

#endif /* GBW_MPOOL_H */
