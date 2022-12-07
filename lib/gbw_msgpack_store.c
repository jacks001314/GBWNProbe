/*
 * =====================================================================================
 *
 *       Filename:  gbw_msgpack_store.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/12/2019 09:54:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "gbw_msgpack_store.h"

gbw_msgpack_store_t* gbw_msgpack_store_create(gbw_pool_t *mp){

    gbw_msgpack_store_t *dstore = gbw_pcalloc(mp,sizeof(*dstore));

    dstore->mp = mp;
    
    msgpack_sbuffer_init(&dstore->pk_buf);
    msgpack_packer_init(&dstore->pk,&dstore->pk_buf,msgpack_sbuffer_write);
             

    return dstore;
}

void gbw_msgpack_store_destroy(gbw_msgpack_store_t *dstore) {


    msgpack_sbuffer_destroy(&dstore->pk_buf);
}

