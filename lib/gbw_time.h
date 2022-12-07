/*
 *
 *      Filename: gbw_time.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 14:26:54
 * Last Modified: 2017-01-11 14:26:54
 */

#ifndef GBW_TIME_H
#define GBW_TIME_H

#include "gbw_constants.h"
#include "gbw_mpool.h"
/** month names */
extern const char gbw_month_snames[12][4];
/** day names */
extern const char gbw_day_snames[7][4];


/** number of microseconds since 00:00:00 January 1, 1970 UTC */
typedef int64_t gbw_time_t;


/** mechanism to properly type gbw_time_t literals */
#define GBW_TIME_C(val) (val ## L)

/** mechanism to properly print gbw_time_t values */
#define GBW_TIME_T_FMT GBW_INT64_T_FMT

/** intervals for I/O timeouts, in microseconds */
typedef int64_t gbw_interval_time_t;
/** short interval for I/O timeouts, in microseconds */
typedef int32_t gbw_short_interval_time_t;

/** number of microseconds per second */
#define GBW_USEC_PER_SEC 1000000L

/** @return gbw_time_t as a second */
#define gbw_time_sec(time) ((time) / GBW_USEC_PER_SEC)

/** @return gbw_time_t as a usec */
#define gbw_time_usec(time) ((time) % GBW_USEC_PER_SEC)

/** @return gbw_time_t as a msec */
#define gbw_time_msec(time) (((time) / 1000) % 1000)

/** @return gbw_time_t as a msec */
#define gbw_time_as_msec(time) ((time) / 1000)

/** @return milliseconds as an gbw_time_t */
#define gbw_time_from_msec(msec) ((gbw_time_t)(msec) * 1000)

/** @return seconds as an gbw_time_t */
#define gbw_time_from_sec(sec) ((gbw_time_t)(sec) * GBW_USEC_PER_SEC)

/** @return a second and usec combination as an gbw_time_t */
#define gbw_time_make(sec, usec) ((gbw_time_t)(sec) * GBW_USEC_PER_SEC \
                                + (gbw_time_t)(usec))

/**
 * @return the current time
 */
gbw_time_t gbw_time_now(void);

/** @see gbw_time_exp_t */
typedef struct gbw_time_exp_t gbw_time_exp_t;

/**
 * a structure similar to ANSI struct tm with the following differences:
 *  - tm_usec isn't an ANSI field
 *  - tm_gmtoff isn't an ANSI field (it's a BSDism)
 */
struct gbw_time_exp_t {
    /** microseconds past tm_sec */
    int32_t tm_usec;
    /** (0-61) seconds past tm_min */
    int32_t tm_sec;
    /** (0-59) minutes past tm_hour */
    int32_t tm_min;
    /** (0-23) hours past midnight */
    int32_t tm_hour;
    /** (1-31) day of the month */
    int32_t tm_mday;
    /** (0-11) month of the year */
    int32_t tm_mon;
    /** year since 1900 */
    int32_t tm_year;
    /** (0-6) days since Sunday */
    int32_t tm_wday;
    /** (0-365) days since January 1 */
    int32_t tm_yday;
    /** daylight saving time */
    int32_t tm_isdst;
    /** seconds east of UTC */
    int32_t tm_gmtoff;
};

/**
 * Convert an ansi time_t to an gbw_time_t
 * @param result the resulting gbw_time_t
 * @param input the time_t to convert
 */
int gbw_time_ansi_put(gbw_time_t *result, 
                                                    time_t input);

/**
 * Convert a time to its human readable components using an offset
 * from GMT.
 * @param result the exploded time
 * @param input the time to explode
 * @param offs the number of seconds offset to apply
 */
int gbw_time_exp_tz(gbw_time_exp_t *result,
                                          gbw_time_t input,
                                          int32_t offs);

/**
 * Convert a time to its human readable components (GMT).
 * @param result the exploded time
 * @param input the time to explode
 */
int gbw_time_exp_gmt(gbw_time_exp_t *result, 
                                           gbw_time_t input);

/**
 * Convert a time to its human readable components in the local timezone.
 * @param result the exploded time
 * @param input the time to explode
 */
int gbw_time_exp_lt(gbw_time_exp_t *result, 
                                          gbw_time_t input);

/**
 * Convert time value from human readable format to a numeric gbw_time_t
 * (elapsed microseconds since the epoch).
 * @param result the resulting imploded time
 * @param input the input exploded time
 */
int gbw_time_exp_get(gbw_time_t *result, 
                                           gbw_time_exp_t *input);

/**
 * Convert time value from human readable format to a numeric gbw_time_t that
 * always represents GMT.
 * @param result the resulting imploded time
 * @param input the input exploded time
 */
int gbw_time_exp_gmt_get(gbw_time_t *result, 
                                               gbw_time_exp_t *input);

/**
 * Sleep for the specified number of micro-seconds.
 * @param t desired amount of time to sleep.
 * @warning May sleep for longer than the specified time. 
 */
void gbw_sleep(gbw_interval_time_t t);

/** length of a RFC822 Date */
#define GBW_RFC822_DATE_LEN (30)
/**
 * gbw_rfc822_date formats dates in the RFC822
 * format in an efficient manner.  It is a fixed length
 * format which requires GBW_RFC822_DATA_LEN bytes of storage,
 * including the trailing NUL terminator.
 * @param date_str String to write to.
 * @param t the time to convert 
 */
int gbw_rfc822_date(char *date_str, gbw_time_t t);

/** length of a CTIME date */
#define GBW_CTIME_LEN (25)
/**
 * gbw_ctime formats dates in the ctime() format
 * in an efficient manner.  It is a fixed length format
 * and requires GBW_CTIME_LEN bytes of storage including
 * the trailing NUL terminator.
 * Unlike ANSI/ISO C ctime(), gbw_ctime() does not include
 * a \\n at the end of the string.
 * @param date_str String to write to.
 * @param t the time to convert 
 */
int gbw_ctime(char *date_str, gbw_time_t t);

/**
 * Formats the exploded time according to the format specified
 * @param s string to write to
 * @param retsize The length of the returned string
 * @param max The maximum length of the string
 * @param format The format for the time string
 * @param tm The time to convert
 */
int gbw_strftime(char *s, size_t *retsize, 
                                       size_t max, const char *format, 
                                       gbw_time_exp_t *tm);

/**
 * Improve the clock resolution for the lifetime of the given pool.
 * Generally this is only desirable on benchmarking and other very
 * time-sensitive applications, and has no impact on most platforms.
 * @param p The pool to associate the finer clock resolution 
 */
void gbw_time_clock_hires(gbw_pool_t *p);


#endif /* GBW_TIME_H */
