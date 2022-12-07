/*
 *
 *      Filename: gbw_object_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-27 13:52:28
 * Last Modified: 2017-12-27 13:52:28
 */

#ifndef GBW_OBJECT_POOL_H
#define GBW_OBJECT_POOL_H

typedef struct gbw_object_pool_t gbw_object_pool_t;
typedef struct gbw_object_mem_t gbw_object_mem_t;
typedef struct gbw_object_item_t gbw_object_item_t;

#include "gbw_mpool.h"
#include "gbw_list.h"

#define OBJECT_MEM_SIZE 64*1024

struct gbw_object_pool_t {

	gbw_object_mem_t *mm_list;

	struct list_head free_list;

	void *priv_data;

	void (*obj_init)(void *obj,void *priv_data);

	size_t object_limits;
	size_t object_size;

	size_t n_objects;

	size_t n_mms;

	size_t n_frees;
	
	size_t n_using;
};

struct gbw_object_mem_t {

	gbw_object_mem_t *next;
};

struct gbw_object_item_t {

	struct list_head node;
};


extern gbw_object_pool_t * gbw_object_pool_create(gbw_pool_t *mp,size_t object_limits,
	size_t object_size,
	void *priv_data,
	void (*obj_init)(void *obj,void *priv_data));

extern void gbw_object_pool_destroy(gbw_object_pool_t *omp);

extern void * gbw_object_pool_get(gbw_object_pool_t *omp);

extern void  gbw_object_pool_put(gbw_object_pool_t *omp,void *obj);

extern void gbw_object_pool_dump(gbw_object_pool_t *omp,FILE *fp);

#endif /*GBW_OBJECT_POOL_H*/
