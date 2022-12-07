/*
 *
 *      Filename: gbw_getopt.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 16:31:23
 * Last Modified: 2017-01-11 16:31:23
 */

#ifndef GBW_GETOPT_H
#define GBW_GETOPT_H

typedef void (gbw_getopt_err_fn_t)(void *arg, const char *err, ...);

/** @see gbw_getopt_t */
typedef struct gbw_getopt_t gbw_getopt_t;

#include "gbw_mpool.h"

/**
 * Structure to store command line argument information.
 */ 
struct gbw_getopt_t {
    /** context for processing */
    gbw_pool_t *cont;
    /** function to print error message (NULL == no messages) */
    gbw_getopt_err_fn_t *errfn;
    /** user defined first arg to pass to error message  */
    void *errarg;
    /** index into parent argv vector */
    int ind;
    /** character checked for validity */
    int opt;
    /** reset getopt */
    int reset;
    /** count of arguments */
    int argc;
    /** array of pointers to arguments */
    const char **argv;
    /** argument associated with option */
    char const* place;
    /** set to nonzero to support interleaving options with regular args */
    int interleave;
    /** start of non-option arguments skipped for interleaving */
    int skip_start;
    /** end of non-option arguments skipped for interleaving */
    int skip_end;
};

/** @see gbw_getopt_option_t */
typedef struct gbw_getopt_option_t gbw_getopt_option_t;

/**
 * Structure used to describe options that getopt should search for.
 */
struct gbw_getopt_option_t {
    /** long option name, or NULL if option has no long name */
    const char *name;
    /** option letter, or a value greater than 255 if option has no letter */
    int optch;
    /** nonzero if option takes an argument */
    int has_arg;
    /** a description of the option */
    const char *description;
};

/**
 * Initialize the arguments for parsing by gbw_getopt().
 * @param os   The options structure created for gbw_getopt()
 * @param cont The pool to operate on
 * @param argc The number of arguments to parse
 * @param argv The array of arguments to parse
 * @remark Arguments 3 and 4 are most commonly argc and argv from main(argc, argv)
 * The (*os)->errfn is initialized to fprintf(stderr... but may be overridden.
 */
int gbw_getopt_init(gbw_getopt_t **os, gbw_pool_t *cont,
                                      int argc, const char * const *argv);

/**
 * Parse the options initialized by gbw_getopt_init().
 * @param os     The gbw_opt_t structure returned by gbw_getopt_init()
 * @param opts   A string of characters that are acceptable options to the 
 *               program.  Characters followed by ":" are required to have an 
 *               option associated
 * @param option_ch  The next option character parsed
 * @param option_arg The argument following the option character:
 * @return There are four potential status values on exit. They are:
 * <PRE>
 *             GBW_EOF      --  No more options to parse
 *             GBW_BADCH    --  Found a bad option character
 *             GBW_BADARG   --  No argument followed the option flag
 *             GBW_SUCCESS  --  The next option was found.
 * </PRE>
 */
int gbw_getopt(gbw_getopt_t *os, const char *opts, 
                                     char *option_ch, const char **option_arg);

/**
 * Parse the options initialized by gbw_getopt_init(), accepting long
 * options beginning with "--" in addition to single-character
 * options beginning with "-".
 * @param os     The gbw_getopt_t structure created by gbw_getopt_init()
 * @param opts   A pointer to a list of gbw_getopt_option_t structures, which
 *               can be initialized with { "name", optch, has_args }.  has_args
 *               is nonzero if the option requires an argument.  A structure
 *               with an optch value of 0 terminates the list.
 * @param option_ch  Receives the value of "optch" from the gbw_getopt_option_t
 *                   structure corresponding to the next option matched.
 * @param option_arg Receives the argument following the option, if any.
 * @return There are four potential status values on exit.   They are:
 * <PRE>
 *             GBW_EOF      --  No more options to parse
 *             GBW_BADCH    --  Found a bad option character
 *             GBW_BADARG   --  No argument followed the option flag
 *             GBW_SUCCESS  --  The next option was found.
 * </PRE>
 * When GBW_SUCCESS is returned, os->ind gives the index of the first
 * non-option argument.  On error, a message will be printed to stdout unless
 * os->err is set to 0.  If os->interleave is set to nonzero, options can come
 * after arguments, and os->argv will be permuted to leave non-option arguments
 * at the end (the original argv is unaffected).
 */
int gbw_getopt_long(gbw_getopt_t *os,
					  const gbw_getopt_option_t *opts,
					  int *option_ch,
                                          const char **option_arg);

#endif /* GBW_GETOPT_H */
