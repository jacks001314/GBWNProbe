/*
 *
 *      Filename: gbw_log.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-18 10:14:56
 * Last Modified: 2017-01-18 10:14:56
 */

#ifndef GBW_LOG_H
#define GBW_LOG_H

#include "gbw_file.h"
#include "gbw_errno.h"

#define GBW_LOG_STDERR            0
#define GBW_LOG_EMERG             1
#define GBW_LOG_ALERT             2
#define GBW_LOG_CRIT              3
#define GBW_LOG_ERR               4
#define GBW_LOG_WARN              5
#define GBW_LOG_NOTICE            6
#define GBW_LOG_INFO              7
#define GBW_LOG_DEBUG             8

typedef struct {
    int       log_level;
    gbw_file_t     *file;
    gbw_pool_t     *mp;
}gbw_log_t;


#define GBW_MAX_ERROR_STR   2048

#define gbw_log(level,fmt,...) gbw_log_error_core(level,0,fmt,##__VA_ARGS__)

#define GBW_DEBUG_LOG(fmt, ...) gbw_log_error_core(GBW_LOG_DEBUG,0,fmt,##__VA_ARGS__)

#define GBW_INFO_LOG(fmt, ...) gbw_log_error_core(GBW_LOG_INFO,0,fmt,##__VA_ARGS__)

#define GBW_NOTICE_LOG(fmt, ...) gbw_log_error_core(GBW_LOG_NOTICE,0,fmt,##__VA_ARGS__)

#define GBW_WARN_LOG(fmt, ...) gbw_log_error_core(GBW_LOG_WARN,0,fmt,##__VA_ARGS__)

#define GBW_ERR_LOG(fmt, ...) gbw_log_error_core(GBW_LOG_ERR,0,fmt,##__VA_ARGS__)

#define GBW_CRIT_LOG(fmt, ...) gbw_log_error_core(GBW_LOG_CRIT,0,fmt,##__VA_ARGS__)

#define GBW_ALERT_LOG(fmt, ...) gbw_log_error_core(GBW_LOG_ALERT,0,fmt,##__VA_ARGS__)

#define GBW_EMERG_LOG(fmt, ...) gbw_log_error_core(GBW_LOG_EMERG,0,fmt,##__VA_ARGS__)

void gbw_log_error_core(int level,int err,
    const char *fmt, ...);


void gbw_log_init(gbw_pool_t *mp,const char *name,int level);

static inline void
gbw_write_stderr(char *text)
{
    fprintf(stderr, text, strlen(text));
}


#endif /* GBW_LOG_H */
