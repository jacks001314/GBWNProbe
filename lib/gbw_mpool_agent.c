#include <limits.h>
#include "gbw_mpool_agent.h"

int gbw_mpool_agent_init(gbw_mpool_agent_t *mpa,unsigned long max_cache_n,unsigned long pool_size,unsigned long pre_alloc_n){

    unsigned long i = 0;

    gbw_pool_t *mp;

    INIT_LIST_HEAD(&mpa->cache_list);
    mpa->max_cache_n = max_cache_n==0?100000:max_cache_n;
    mpa->cur_cache_n = 0;
    mpa->pool_size = pool_size == 0?4096:pool_size;
    mpa->pre_alloc_n = pre_alloc_n==0?1000:pre_alloc_n;

    mpa->from_mems = 0;
    mpa->from_caches = 0;

    mpa->allocs = 0;
    mpa->frees = 0;

    for(i = 0;i<mpa->pre_alloc_n;i++){


        mp = gbw_pool_create(mpa->pool_size);

        if(mp){

            mpa->cur_cache_n = mpa->cur_cache_n+1;
            list_add(&mp->node,&mpa->cache_list);

        }
    }

    return 0;

}

gbw_mpool_agent_t *gbw_mpool_agent_create(gbw_pool_t *mp,
        unsigned long max_cache_n,unsigned long pool_size,
        unsigned long pre_alloc_n){


    gbw_mpool_agent_t *mpa = (gbw_mpool_agent_t*)gbw_pcalloc(mp,sizeof(*mpa));

    gbw_mpool_agent_init(mpa,max_cache_n,pool_size,pre_alloc_n);

    return mpa;
}

gbw_pool_t *gbw_mpool_agent_alloc(gbw_mpool_agent_t *mpa){


    gbw_pool_t *mp = NULL;

    if(!list_empty(&mpa->cache_list)){
        
        mp = list_first_entry(&mpa->cache_list,gbw_pool_t,node);
        
        list_del(&mp->node);
        mpa->cur_cache_n = mpa->cur_cache_n-1;
        mpa->from_caches = (mpa->from_caches+1)%ULONG_MAX;
    }else{

        mp = gbw_pool_create(mpa->pool_size);
        mpa->from_mems = (mpa->from_mems+1)%ULONG_MAX;
    }

    if(mp){

        gbw_pool_reset(mp);
    }

    mpa->allocs = (mpa->allocs+1)%ULONG_MAX;
    return mp;
}

void gbw_mpool_agent_free(gbw_mpool_agent_t *mpa,gbw_pool_t *mp){

    if(mpa->cur_cache_n<mpa->max_cache_n){

        gbw_pool_reset(mp);
        list_add(&mp->node,&mpa->cache_list);
        mpa->cur_cache_n = mpa->cur_cache_n+1;

    }else{

        gbw_pool_destroy(mp);
    }

    mpa->frees = (mpa->frees+1)%ULONG_MAX;
}

void gbw_mpool_agent_log(gbw_mpool_agent_t *mpa){


    gbw_log(GBW_LOG_INFO,"MaxCaches:%lu,CurCaches:%lu,PSize:%lu,PreAllocs:%lu,FromCaches:%lu,FromMems:%lu,allocs:%lu,frees:%lu",
            mpa->max_cache_n,mpa->cur_cache_n,mpa->pool_size,mpa->pre_alloc_n,mpa->from_caches,mpa->from_mems,mpa->allocs,mpa->frees);

} 

void gbw_mpool_agent_dump(gbw_mpool_agent_t *mpa,FILE *out){


    fprintf(out,"MaxCaches:%lu,CurCaches:%lu,PSize:%lu,PreAllocs:%lu,FromCaches:%lu,FromMems:%lu,allocs:%lu,frees:%lu\n",
            mpa->max_cache_n,mpa->cur_cache_n,mpa->pool_size,mpa->pre_alloc_n,mpa->from_caches,mpa->from_mems,mpa->allocs,mpa->frees);


    fflush(out);
} 

