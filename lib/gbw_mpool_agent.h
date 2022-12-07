/*
 * =====================================================================================
 *
 *       Filename:  gbw_mpool_agent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/12/2018 04:31:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef GBW_MPOOL_AGNENT_H
#define GBW_MPOOL_AGNENT_H

typedef struct gbw_mpool_agent_t gbw_mpool_agent_t;

#include "gbw_mpool.h"
#include "gbw_list.h"
#include "gbw_log.h"

struct gbw_mpool_agent_t {

    struct list_head cache_list; 

    unsigned long max_cache_n;
    unsigned long cur_cache_n;

    unsigned long pool_size;
    unsigned long pre_alloc_n;

    unsigned long from_caches;
    unsigned long from_mems;

    unsigned long allocs;
    unsigned long frees;
};

extern int gbw_mpool_agent_init(gbw_mpool_agent_t *mpa,
        unsigned long max_cache_n,unsigned long pool_size,unsigned long pre_alloc_n);


extern gbw_mpool_agent_t *gbw_mpool_agent_create(gbw_pool_t *mp,
        unsigned long max_cache_n,unsigned long pool_size,
        unsigned long pre_alloc_n);


extern gbw_pool_t *gbw_mpool_agent_alloc(gbw_mpool_agent_t *mpa);

extern void gbw_mpool_agent_free(gbw_mpool_agent_t *mpa,gbw_pool_t *mp);

extern void gbw_mpool_agent_log(gbw_mpool_agent_t *mpa);


extern void gbw_mpool_agent_dump(gbw_mpool_agent_t *mpa,FILE *out);

#endif /* GBW_MPOOL_AGNENT_H */
