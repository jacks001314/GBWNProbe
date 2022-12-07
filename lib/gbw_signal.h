/*
 *
 *      Filename: gbw_signal.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 16:48:01
 * Last Modified: 2017-01-11 16:48:01
 */

#ifndef GBW_SIGNAL_H
#define GBW_SIGNAL_H

#include "gbw_mpool.h"

/** Function prototype for signal handlers */
typedef void gbw_sigfunc_t(int);

/**
 * Set the signal handler function for a given signal
 * @param signo The signal (eg... SIGWINCH)
 * @param func the function to get called
 */
gbw_sigfunc_t * gbw_signal(int signo, gbw_sigfunc_t * func);

#if defined(SIG_IGN) && !defined(SIG_ERR)
#define SIG_ERR ((gbw_sigfunc_t *) -1)
#endif



/**
 * Get the description for a specific signal number
 * @param signum The signal number
 * @return The description of the signal
 */
const char * gbw_signal_description_get(int signum);

/**
 * GW-private function for initializing the signal package
 * @internal
 * @param pglobal The internal, global pool
 */
void gbw_signal_init(gbw_pool_t *pglobal);

/**
 * Block the delivery of a particular signal
 * @param signum The signal number
 * @return status
 */
int gbw_signal_block(int signum);

/**
 * Enable the delivery of a particular signal
 * @param signum The signal number
 * @return status
 */
int gbw_signal_unblock(int signum);


#endif /* GBW_SIGNAL_H */
