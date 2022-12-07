/*
 *
 *      Filename: gbw_errno.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 10:08:19
 * Last Modified: 2017-01-11 10:08:19
 */

#ifndef GBW_ERRNO_H
#define GBW_ERRNO_H

#include <errno.h>
#include "gbw_constants.h"

extern char * gbw_strerror(int statcode, char *buf,
                                 size_t bufsize);

/**
 * GBW_OS_START_ERROR is where the GW specific error values start.
 */
#define GBW_OS_START_ERROR     20000
/**
 * GBW_OS_ERRSPACE_SIZE is the maximum number of errors you can fit
 *    into one of the error/status ranges below -- except for
 *    GBW_OS_START_USERERR, which see.
 */
#define GBW_OS_ERRSPACE_SIZE 50000
/**
 * GBW_UTIL_ERRSPACE_SIZE is the size of the space that is reserved for
 * use within gw-util. This space is reserved above that used by GW
 * internally.
 * @note This number MUST be smaller than GBW_OS_ERRSPACE_SIZE by a
 *       large enough amount that GW has sufficient room for its
 *       codes.
 */
#define GBW_UTIL_ERRSPACE_SIZE 20000
/**
 * GBW_OS_START_STATUS is where the GW specific status codes start.
 */
#define GBW_OS_START_STATUS    (GBW_OS_START_ERROR + GBW_OS_ERRSPACE_SIZE)
/**
 * GBW_UTIL_START_STATUS is where GW-Util starts defining its
 * status codes.
 */
#define GBW_UTIL_START_STATUS   (GBW_OS_START_STATUS + \
                           (GBW_OS_ERRSPACE_SIZE - GBW_UTIL_ERRSPACE_SIZE))
/**
 * GBW_OS_START_USERERR are reserved for applications that use GW that
 *     layer their own error codes along with GW's.  Note that the
 *     error immediately following this one is set ten times farther
 *     away than usual, so that users of gw have a lot of room in
 *     which to declare custom error codes.
 *
 * In general applications should try and create unique error codes. To try
 * and assist in finding suitable ranges of numbers to use, the following
 * ranges are known to be used by the listed applications. If your
 * application defines error codes please advise the range of numbers it
 * uses to dev@gw.apache.org for inclusion in this list.
 *
 * Ranges shown are in relation to GBW_OS_START_USERERR
 *
 * Subversion - Defined ranges, of less than 100, at intervals of 5000
 *              starting at an offset of 5000, e.g.
 *               +5000 to 5100,  +10000 to 10100
 *
 * Apache HTTPD - +2000 to 2999
 */
#define GBW_OS_START_USERERR    (GBW_OS_START_STATUS + GBW_OS_ERRSPACE_SIZE)
/**
 * GBW_OS_START_USEERR is obsolete, defined for compatibility only.
 * Use GBW_OS_START_USERERR instead.
 */
#define GBW_OS_START_USEERR     GBW_OS_START_USERERR
/**
 * GBW_OS_START_CANONERR is where GW versions of errno values are defined
 *     on systems which don't have the corresponding errno.
 */
#define GBW_OS_START_CANONERR  (GBW_OS_START_USERERR \
                                 + (GBW_OS_ERRSPACE_SIZE * 10))
/**
 * GBW_OS_START_EAIERR folds EAI_ error codes from getaddrinfo() into
 *     gbw_status_t values.
 */
#define GBW_OS_START_EAIERR    (GBW_OS_START_CANONERR + GBW_OS_ERRSPACE_SIZE)
/**
 * GBW_OS_START_SYSERR folds platform-specific system error values into
 *     gbw_status_t values.
 */
#define GBW_OS_START_SYSERR    (GBW_OS_START_EAIERR + GBW_OS_ERRSPACE_SIZE)

/**
 * @defgroup GBW_ERROR_map GW Error Space
 * <PRE>
 * The following attempts to show the relation of the various constants
 * used for mapping GW Status codes.
 *
 *       0
 *
 *  20,000     GBW_OS_START_ERROR
 *
 *         + GBW_OS_ERRSPACE_SIZE (50,000)
 *
 *  70,000      GBW_OS_START_STATUS
 *
 *         + GBW_OS_ERRSPACE_SIZE - GBW_UTIL_ERRSPACE_SIZE (30,000)
 *
 * 100,000      GBW_UTIL_START_STATUS
 *
 *         + GBW_UTIL_ERRSPACE_SIZE (20,000)
 *
 * 120,000      GBW_OS_START_USERERR
 *
 *         + 10 x GBW_OS_ERRSPACE_SIZE (50,000 * 10)
 *
 * 620,000      GBW_OS_START_CANONERR
 *
 *         + GBW_OS_ERRSPACE_SIZE (50,000)
 *
 * 670,000      GBW_OS_START_EAIERR
 *
 *         + GBW_OS_ERRSPACE_SIZE (50,000)
 *
 * 720,000      GBW_OS_START_SYSERR
 *
 * </PRE>
 */

/** no error. */
#define GBW_OK 0

/**
 * @defgroup GBW_Error GW Error Values
 * <PRE>
 * <b>GW ERROR VALUES</b>
 * GBW_ENOSTAT      GW was unable to perform a stat on the file
 * GBW_ENOPOOL      GW was not provided a pool with which to allocate memory
 * GBW_EBADDATE     GW was given an invalid date
 * GBW_EINVALSOCK   GW was given an invalid socket
 * GBW_ENOPROC      GW was not given a process structure
 * GBW_ENOTIME      GW was not given a time structure
 * GBW_ENODIR       GW was not given a directory structure
 * GBW_ENOLOCK      GW was not given a lock structure
 * GBW_ENOPOLL      GW was not given a poll structure
 * GBW_ENOSOCKET    GW was not given a socket
 * GBW_ENOTHREAD    GW was not given a thread structure
 * GBW_ENOTHDKEY    GW was not given a thread key structure
 * GBW_ENOSHMAVAIL  There is no more shared memory available
 * GBW_EDSOOPEN     GW was unable to open the dso object.  For more
 *                  information call gbw_dso_error().
 * GBW_EGENERAL     General failure (specific information not available)
 * GBW_EBADIP       The specified IP address is invalid
 * GBW_EBADMASK     The specified netmask is invalid
 * GBW_ESYMNOTFOUND Could not find the requested symbol
 * GBW_ENOTENOUGHENTROPY Not enough entropy to continue
 * </PRE>
 *
 * <PRE>
 * <b>GW STATUS VALUES</b>
 * GBW_INCHILD        Program is currently executing in the child
 * GBW_INPARENT       Program is currently executing in the parent
 * GBW_DETACH         The thread is detached
 * GBW_NOTDETACH      The thread is not detached
 * GBW_CHILD_DONE     The child has finished executing
 * GBW_CHILD_NOTDONE  The child has not finished executing
 * GBW_TIMEUP         The operation did not finish before the timeout
 * GBW_INCOMPLETE     The operation was incomplete although some processing
 *                    was performed and the results are partially valid
 * GBW_BADCH          Getopt found an option not in the option string
 * GBW_BADARG         Getopt found an option that is missing an argument
 *                    and an argument was specified in the option string
 * GBW_EOF            GW has encountered the end of the file
 * GBW_NOTFOUND       GW was unable to find the socket in the poll structure
 * GBW_ANONYMOUS      GW is using anonymous shared memory
 * GBW_FILEBASED      GW is using a file name as the key to the shared memory
 * GBW_KEYBASED       GW is using a shared key as the key to the shared memory
 * GBW_EINIT          Ininitalizer value.  If no option has been found, but
 *                    the status variable requires a value, this should be used
 * GBW_ENOTIMPL       The GW function has not been implemented on this
 *                    platform, either because nobody has gotten to it yet,
 *                    or the function is impossible on this platform.
 * GBW_EMISMATCH      Two passwords do not match.
 * GBW_EABSOLUTE      The given path was absolute.
 * GBW_ERELATIVE      The given path was relative.
 * GBW_EINCOMPLETE    The given path was neither relative nor absolute.
 * GBW_EABOVEROOT     The given path was above the root path.
 * GBW_EBUSY          The given lock was busy.
 * GBW_EPROC_UNKNOWN  The given process wasn't recognized by GW
 * </PRE>
 * @{
 */
/** @see GBW_STATUS_IS_ENOSTAT */
#define GBW_ENOSTAT        (GBW_OS_START_ERROR + 1)
/** @see GBW_STATUS_IS_ENOPOOL */
#define GBW_ENOPOOL        (GBW_OS_START_ERROR + 2)
/* empty slot: +3 */
/** @see GBW_STATUS_IS_EBADDATE */
#define GBW_EBADDATE       (GBW_OS_START_ERROR + 4)
/** @see GBW_STATUS_IS_EINVALSOCK */
#define GBW_EINVALSOCK     (GBW_OS_START_ERROR + 5)
/** @see GBW_STATUS_IS_ENOPROC */
#define GBW_ENOPROC        (GBW_OS_START_ERROR + 6)
/** @see GBW_STATUS_IS_ENOTIME */
#define GBW_ENOTIME        (GBW_OS_START_ERROR + 7)
/** @see GBW_STATUS_IS_ENODIR */
#define GBW_ENODIR         (GBW_OS_START_ERROR + 8)
/** @see GBW_STATUS_IS_ENOLOCK */
#define GBW_ENOLOCK        (GBW_OS_START_ERROR + 9)
/** @see GBW_STATUS_IS_ENOPOLL */
#define GBW_ENOPOLL        (GBW_OS_START_ERROR + 10)
/** @see GBW_STATUS_IS_ENOSOCKET */
#define GBW_ENOSOCKET      (GBW_OS_START_ERROR + 11)
/** @see GBW_STATUS_IS_ENOTHREAD */
#define GBW_ENOTHREAD      (GBW_OS_START_ERROR + 12)
/** @see GBW_STATUS_IS_ENOTHDKEY */
#define GBW_ENOTHDKEY      (GBW_OS_START_ERROR + 13)
/** @see GBW_STATUS_IS_EGENERAL */
#define GBW_EGENERAL       (GBW_OS_START_ERROR + 14)
/** @see GBW_STATUS_IS_ENOSHMAVAIL */
#define GBW_ENOSHMAVAIL    (GBW_OS_START_ERROR + 15)
/** @see GBW_STATUS_IS_EBADIP */
#define GBW_EBADIP         (GBW_OS_START_ERROR + 16)
/** @see GBW_STATUS_IS_EBADMASK */
#define GBW_EBADMASK       (GBW_OS_START_ERROR + 17)
/* empty slot: +18 */
/** @see GBW_STATUS_IS_EDSOPEN */
#define GBW_EDSOOPEN       (GBW_OS_START_ERROR + 19)
/** @see GBW_STATUS_IS_EABSOLUTE */
#define GBW_EABSOLUTE      (GBW_OS_START_ERROR + 20)
/** @see GBW_STATUS_IS_ERELATIVE */
#define GBW_ERELATIVE      (GBW_OS_START_ERROR + 21)
/** @see GBW_STATUS_IS_EINCOMPLETE */
#define GBW_EINCOMPLETE    (GBW_OS_START_ERROR + 22)
/** @see GBW_STATUS_IS_EABOVEROOT */
#define GBW_EABOVEROOT     (GBW_OS_START_ERROR + 23)
/** @see GBW_STATUS_IS_EBADPATH */
#define GBW_EBADPATH       (GBW_OS_START_ERROR + 24)
/** @see GBW_STATUS_IS_EPATHWILD */
#define GBW_EPATHWILD      (GBW_OS_START_ERROR + 25)
/** @see GBW_STATUS_IS_ESYMNOTFOUND */
#define GBW_ESYMNOTFOUND   (GBW_OS_START_ERROR + 26)
/** @see GBW_STATUS_IS_EPROC_UNKNOWN */
#define GBW_EPROC_UNKNOWN  (GBW_OS_START_ERROR + 27)
/** @see GBW_STATUS_IS_ENOTENOUGHENTROPY */
#define GBW_ENOTENOUGHENTROPY (GBW_OS_START_ERROR + 28)
/** @} */

/**
 * @defgroup GBW_STATUS_IS Status Value Tests
 * @warning For any particular error condition, more than one of these tests
 *      may match. This is because platform-specific error codes may not
 *      always match the semantics of the POSIX codes these tests (and the
 *      corresponding GW error codes) are named after. A notable example
 *      are the GBW_STATUS_IS_ENOENT and GBW_STATUS_IS_ENOTDIR tests on
 *      Win32 platforms. The programmer should always be aware of this and
 *      adjust the order of the tests accordingly.
 * @{
 */
/**
 * GW was unable to perform a stat on the file
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_ENOSTAT(s)        ((s) == GBW_ENOSTAT)
/**
 * GW was not provided a pool with which to allocate memory
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_ENOPOOL(s)        ((s) == GBW_ENOPOOL)
/** GW was given an invalid date  */
#define GBW_STATUS_IS_EBADDATE(s)       ((s) == GBW_EBADDATE)
/** GW was given an invalid socket */
#define GBW_STATUS_IS_EINVALSOCK(s)     ((s) == GBW_EINVALSOCK)
/** GW was not given a process structure */
#define GBW_STATUS_IS_ENOPROC(s)        ((s) == GBW_ENOPROC)
/** GW was not given a time structure */
#define GBW_STATUS_IS_ENOTIME(s)        ((s) == GBW_ENOTIME)
/** GW was not given a directory structure */
#define GBW_STATUS_IS_ENODIR(s)         ((s) == GBW_ENODIR)
/** GW was not given a lock structure */
#define GBW_STATUS_IS_ENOLOCK(s)        ((s) == GBW_ENOLOCK)
/** GW was not given a poll structure */
#define GBW_STATUS_IS_ENOPOLL(s)        ((s) == GBW_ENOPOLL)
/** GW was not given a socket */
#define GBW_STATUS_IS_ENOSOCKET(s)      ((s) == GBW_ENOSOCKET)
/** GW was not given a thread structure */
#define GBW_STATUS_IS_ENOTHREAD(s)      ((s) == GBW_ENOTHREAD)
/** GW was not given a thread key structure */
#define GBW_STATUS_IS_ENOTHDKEY(s)      ((s) == GBW_ENOTHDKEY)
/** Generic Error which can not be put into another spot */
#define GBW_STATUS_IS_EGENERAL(s)       ((s) == GBW_EGENERAL)
/** There is no more shared memory available */
#define GBW_STATUS_IS_ENOSHMAVAIL(s)    ((s) == GBW_ENOSHMAVAIL)
/** The specified IP address is invalid */
#define GBW_STATUS_IS_EBADIP(s)         ((s) == GBW_EBADIP)
/** The specified netmask is invalid */
#define GBW_STATUS_IS_EBADMASK(s)       ((s) == GBW_EBADMASK)
/* empty slot: +18 */
/**
 * GW was unable to open the dso object.
 * For more information call gbw_dso_error().
 */
#define GBW_STATUS_IS_EDSOOPEN(s)       ((s) == GBW_EDSOOPEN)
/** The given path was absolute. */
#define GBW_STATUS_IS_EABSOLUTE(s)      ((s) == GBW_EABSOLUTE)
/** The given path was relative. */
#define GBW_STATUS_IS_ERELATIVE(s)      ((s) == GBW_ERELATIVE)
/** The given path was neither relative nor absolute. */
#define GBW_STATUS_IS_EINCOMPLETE(s)    ((s) == GBW_EINCOMPLETE)
/** The given path was above the root path. */
#define GBW_STATUS_IS_EABOVEROOT(s)     ((s) == GBW_EABOVEROOT)
/** The given path was bad. */
#define GBW_STATUS_IS_EBADPATH(s)       ((s) == GBW_EBADPATH)
/** The given path contained wildcards. */
#define GBW_STATUS_IS_EPATHWILD(s)      ((s) == GBW_EPATHWILD)
/** Could not find the requested symbol.
 * For more information call gbw_dso_error().
 */
#define GBW_STATUS_IS_ESYMNOTFOUND(s)   ((s) == GBW_ESYMNOTFOUND)
/** The given process was not recognized by GW. */
#define GBW_STATUS_IS_EPROC_UNKNOWN(s)  ((s) == GBW_EPROC_UNKNOWN)
/** GW could not gather enough entropy to continue. */
#define GBW_STATUS_IS_ENOTENOUGHENTROPY(s) ((s) == GBW_ENOTENOUGHENTROPY)

/** @} */

/**
 * @addtogroup GBW_Error
 * @{
 */
/** @see GBW_STATUS_IS_INCHILD */
#define GBW_INCHILD        (GBW_OS_START_STATUS + 1)
/** @see GBW_STATUS_IS_INPARENT */
#define GBW_INPARENT       (GBW_OS_START_STATUS + 2)
/** @see GBW_STATUS_IS_DETACH */
#define GBW_DETACH         (GBW_OS_START_STATUS + 3)
/** @see GBW_STATUS_IS_NOTDETACH */
#define GBW_NOTDETACH      (GBW_OS_START_STATUS + 4)
/** @see GBW_STATUS_IS_CHILD_DONE */
#define GBW_CHILD_DONE     (GBW_OS_START_STATUS + 5)
/** @see GBW_STATUS_IS_CHILD_NOTDONE */
#define GBW_CHILD_NOTDONE  (GBW_OS_START_STATUS + 6)
/** @see GBW_STATUS_IS_TIMEUP */
#define GBW_TIMEUP         (GBW_OS_START_STATUS + 7)
/** @see GBW_STATUS_IS_INCOMPLETE */
#define GBW_INCOMPLETE     (GBW_OS_START_STATUS + 8)
/* empty slot: +9 */
/* empty slot: +10 */
/* empty slot: +11 */
/** @see GBW_STATUS_IS_BADCH */
#define GBW_BADCH          (GBW_OS_START_STATUS + 12)
/** @see GBW_STATUS_IS_BADARG */
#define GBW_BADARG         (GBW_OS_START_STATUS + 13)
/** @see GBW_STATUS_IS_EOF */
#define GBW_EOF            (GBW_OS_START_STATUS + 14)
/** @see GBW_STATUS_IS_NOTFOUND */
#define GBW_NOTFOUND       (GBW_OS_START_STATUS + 15)
/* empty slot: +16 */
/* empty slot: +17 */
/* empty slot: +18 */
/** @see GBW_STATUS_IS_ANONYMOUS */
#define GBW_ANONYMOUS      (GBW_OS_START_STATUS + 19)
/** @see GBW_STATUS_IS_FILEBASED */
#define GBW_FILEBASED      (GBW_OS_START_STATUS + 20)
/** @see GBW_STATUS_IS_KEYBASED */
#define GBW_KEYBASED       (GBW_OS_START_STATUS + 21)
/** @see GBW_STATUS_IS_EINIT */
#define GBW_EINIT          (GBW_OS_START_STATUS + 22)
/** @see GBW_STATUS_IS_ENOTIMPL */
#define GBW_ENOTIMPL       (GBW_OS_START_STATUS + 23)
/** @see GBW_STATUS_IS_EMISMATCH */
#define GBW_EMISMATCH      (GBW_OS_START_STATUS + 24)
/** @see GBW_STATUS_IS_EBUSY */
#define GBW_EBUSY          (GBW_OS_START_STATUS + 25)
/** @} */

/**
 * @addtogroup GBW_STATUS_IS
 * @{
 */
/**
 * Program is currently executing in the child
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code */
#define GBW_STATUS_IS_INCHILD(s)        ((s) == GBW_INCHILD)
/**
 * Program is currently executing in the parent
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_INPARENT(s)       ((s) == GBW_INPARENT)
/**
 * The thread is detached
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_DETACH(s)         ((s) == GBW_DETACH)
/**
 * The thread is not detached
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_NOTDETACH(s)      ((s) == GBW_NOTDETACH)
/**
 * The child has finished executing
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_CHILD_DONE(s)     ((s) == GBW_CHILD_DONE)
/**
 * The child has not finished executing
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_CHILD_NOTDONE(s)  ((s) == GBW_CHILD_NOTDONE)
/**
 * The operation did not finish before the timeout
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_TIMEUP(s)         ((s) == GBW_TIMEUP)
/**
 * The operation was incomplete although some processing was performed
 * and the results are partially valid.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_INCOMPLETE(s)     ((s) == GBW_INCOMPLETE)
/* empty slot: +9 */
/* empty slot: +10 */
/* empty slot: +11 */
/**
 * Getopt found an option not in the option string
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_BADCH(s)          ((s) == GBW_BADCH)
/**
 * Getopt found an option not in the option string and an argument was
 * specified in the option string
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_BADARG(s)         ((s) == GBW_BADARG)
/**
 * GW has encountered the end of the file
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_EOF(s)            ((s) == GBW_EOF)
/**
 * GW was unable to find the socket in the poll structure
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_NOTFOUND(s)       ((s) == GBW_NOTFOUND)
/* empty slot: +16 */
/* empty slot: +17 */
/* empty slot: +18 */
/**
 * GW is using anonymous shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_ANONYMOUS(s)      ((s) == GBW_ANONYMOUS)
/**
 * GW is using a file name as the key to the shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_FILEBASED(s)      ((s) == GBW_FILEBASED)
/**
 * GW is using a shared key as the key to the shared memory
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_KEYBASED(s)       ((s) == GBW_KEYBASED)
/**
 * Ininitalizer value.  If no option has been found, but
 * the status variable requires a value, this should be used
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_EINIT(s)          ((s) == GBW_EINIT)
/**
 * The GW function has not been implemented on this
 * platform, either because nobody has gotten to it yet,
 * or the function is impossible on this platform.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_ENOTIMPL(s)       ((s) == GBW_ENOTIMPL)
/**
 * Two passwords do not match.
 * @warning
 * always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_EMISMATCH(s)      ((s) == GBW_EMISMATCH)
/**
 * The given lock was busy
 * @warning always use this test, as platform-specific variances may meet this
 * more than one error code
 */
#define GBW_STATUS_IS_EBUSY(s)          ((s) == GBW_EBUSY)

/** @} */

/**
 * @addtogroup GBW_Error GW Error Values
 * @{
 */
/* GW CANONICAL ERROR VALUES */
/** @see GBW_STATUS_IS_EACCES */
#ifdef EACCES
#define GBW_EACCES EACCES
#else
#define GBW_EACCES         (GBW_OS_START_CANONERR + 1)
#endif

/** @see GBW_STATUS_IS_EEXIST */
#ifdef EEXIST
#define GBW_EEXIST EEXIST
#else
#define GBW_EEXIST         (GBW_OS_START_CANONERR + 2)
#endif

/** @see GBW_STATUS_IS_ENAMETOOLONG */
#ifdef ENAMETOOLONG
#define GBW_ENAMETOOLONG ENAMETOOLONG
#else
#define GBW_ENAMETOOLONG   (GBW_OS_START_CANONERR + 3)
#endif

/** @see GBW_STATUS_IS_ENOENT */
#ifdef ENOENT
#define GBW_ENOENT ENOENT
#else
#define GBW_ENOENT         (GBW_OS_START_CANONERR + 4)
#endif

/** @see GBW_STATUS_IS_ENOTDIR */
#ifdef ENOTDIR
#define GBW_ENOTDIR ENOTDIR
#else
#define GBW_ENOTDIR        (GBW_OS_START_CANONERR + 5)
#endif

/** @see GBW_STATUS_IS_ENOSPC */
#ifdef ENOSPC
#define GBW_ENOSPC ENOSPC
#else
#define GBW_ENOSPC         (GBW_OS_START_CANONERR + 6)
#endif

/** @see GBW_STATUS_IS_ENOMEM */
#ifdef ENOMEM
#define GBW_ENOMEM ENOMEM
#else
#define GBW_ENOMEM         (GBW_OS_START_CANONERR + 7)
#endif

/** @see GBW_STATUS_IS_EMFILE */
#ifdef EMFILE
#define GBW_EMFILE EMFILE
#else
#define GBW_EMFILE         (GBW_OS_START_CANONERR + 8)
#endif

/** @see GBW_STATUS_IS_ENFILE */
#ifdef ENFILE
#define GBW_ENFILE ENFILE
#else
#define GBW_ENFILE         (GBW_OS_START_CANONERR + 9)
#endif

/** @see GBW_STATUS_IS_EBADF */
#ifdef EBADF
#define GBW_EBADF EBADF
#else
#define GBW_EBADF          (GBW_OS_START_CANONERR + 10)
#endif

/** @see GBW_STATUS_IS_EINVAL */
#ifdef EINVAL
#define GBW_EINVAL EINVAL
#else
#define GBW_EINVAL         (GBW_OS_START_CANONERR + 11)
#endif

/** @see GBW_STATUS_IS_ESPIPE */
#ifdef ESPIPE
#define GBW_ESPIPE ESPIPE
#else
#define GBW_ESPIPE         (GBW_OS_START_CANONERR + 12)
#endif

/**
 * @see GBW_STATUS_IS_EAGAIN
 * @warning use GBW_STATUS_IS_EAGAIN instead of just testing this value
 */
#ifdef EAGAIN
#define GBW_EAGAIN EAGAIN
#elif defined(EWOULDBLOCK)
#define GBW_EAGAIN EWOULDBLOCK
#else
#define GBW_EAGAIN         (GBW_OS_START_CANONERR + 13)
#endif

/** @see GBW_STATUS_IS_EINTR */
#ifdef EINTR
#define GBW_EINTR EINTR
#else
#define GBW_EINTR          (GBW_OS_START_CANONERR + 14)
#endif

/** @see GBW_STATUS_IS_ENOTSOCK */
#ifdef ENOTSOCK
#define GBW_ENOTSOCK ENOTSOCK
#else
#define GBW_ENOTSOCK       (GBW_OS_START_CANONERR + 15)
#endif

/** @see GBW_STATUS_IS_ECONNREFUSED */
#ifdef ECONNREFUSED
#define GBW_ECONNREFUSED ECONNREFUSED
#else
#define GBW_ECONNREFUSED   (GBW_OS_START_CANONERR + 16)
#endif

/** @see GBW_STATUS_IS_EINPROGRESS */
#ifdef EINPROGRESS
#define GBW_EINPROGRESS EINPROGRESS
#else
#define GBW_EINPROGRESS    (GBW_OS_START_CANONERR + 17)
#endif

/**
 * @see GBW_STATUS_IS_ECONNABORTED
 * @warning use GBW_STATUS_IS_ECONNABORTED instead of just testing this value
 */

#ifdef ECONNABORTED
#define GBW_ECONNABORTED ECONNABORTED
#else
#define GBW_ECONNABORTED   (GBW_OS_START_CANONERR + 18)
#endif

/** @see GBW_STATUS_IS_ECONNRESET */
#ifdef ECONNRESET
#define GBW_ECONNRESET ECONNRESET
#else
#define GBW_ECONNRESET     (GBW_OS_START_CANONERR + 19)
#endif

/** @see GBW_STATUS_IS_ETIMEDOUT
 *  @deprecated */
#ifdef ETIMEDOUT
#define GBW_ETIMEDOUT ETIMEDOUT
#else
#define GBW_ETIMEDOUT      (GBW_OS_START_CANONERR + 20)
#endif

/** @see GBW_STATUS_IS_EHOSTUNREACH */
#ifdef EHOSTUNREACH
#define GBW_EHOSTUNREACH EHOSTUNREACH
#else
#define GBW_EHOSTUNREACH   (GBW_OS_START_CANONERR + 21)
#endif

/** @see GBW_STATUS_IS_ENETUNREACH */
#ifdef ENETUNREACH
#define GBW_ENETUNREACH ENETUNREACH
#else
#define GBW_ENETUNREACH    (GBW_OS_START_CANONERR + 22)
#endif

/** @see GBW_STATUS_IS_EFTYPE */
#ifdef EFTYPE
#define GBW_EFTYPE EFTYPE
#else
#define GBW_EFTYPE        (GBW_OS_START_CANONERR + 23)
#endif

/** @see GBW_STATUS_IS_EPIPE */
#ifdef EPIPE
#define GBW_EPIPE EPIPE
#else
#define GBW_EPIPE         (GBW_OS_START_CANONERR + 24)
#endif

/** @see GBW_STATUS_IS_EXDEV */
#ifdef EXDEV
#define GBW_EXDEV EXDEV
#else
#define GBW_EXDEV         (GBW_OS_START_CANONERR + 25)
#endif

/** @see GBW_STATUS_IS_ENOTEMPTY */
#ifdef ENOTEMPTY
#define GBW_ENOTEMPTY ENOTEMPTY
#else
#define GBW_ENOTEMPTY     (GBW_OS_START_CANONERR + 26)
#endif

/** @see GBW_STATUS_IS_EAFNOSUPPORT */
#ifdef EAFNOSUPPORT
#define GBW_EAFNOSUPPORT EAFNOSUPPORT
#else
#define GBW_EAFNOSUPPORT  (GBW_OS_START_CANONERR + 27)
#endif

/** @} */

#define GBW_FROM_OS_ERROR(e)  (e)
#define GBW_TO_OS_ERROR(e)    (e)

#define gbw_get_os_error()    (errno)
#define gbw_set_os_error(e)   (errno = (e))

/* A special case, only socket calls require this:
 */
#define gbw_get_netos_error() (errno)
#define gbw_set_netos_error(e) (errno = (e))

/**
 * @addtogroup GBW_STATUS_IS
 * @{
 */

/** permission denied */
#define GBW_STATUS_IS_EACCES(s)         ((s) == GBW_EACCES)
/** file exists */
#define GBW_STATUS_IS_EEXIST(s)         ((s) == GBW_EEXIST)
/** path name is too long */
#define GBW_STATUS_IS_ENAMETOOLONG(s)   ((s) == GBW_ENAMETOOLONG)
/**
 * no such file or directory
 * @remark
 * EMVSCATLG can be returned by the automounter on z/OS for
 * paths which do not exist.
 */
#ifdef EMVSCATLG
#define GBW_STATUS_IS_ENOENT(s)         ((s) == GBW_ENOENT \
                                      || (s) == EMVSCATLG)
#else
#define GBW_STATUS_IS_ENOENT(s)         ((s) == GBW_ENOENT)
#endif
/** not a directory */
#define GBW_STATUS_IS_ENOTDIR(s)        ((s) == GBW_ENOTDIR)
/** no space left on device */
#ifdef EDQUOT
#define GBW_STATUS_IS_ENOSPC(s)         ((s) == GBW_ENOSPC \
                                      || (s) == EDQUOT)
#else
#define GBW_STATUS_IS_ENOSPC(s)         ((s) == GBW_ENOSPC)
#endif
/** not enough memory */
#define GBW_STATUS_IS_ENOMEM(s)         ((s) == GBW_ENOMEM)
/** too many open files */
#define GBW_STATUS_IS_EMFILE(s)         ((s) == GBW_EMFILE)
/** file table overflow */
#define GBW_STATUS_IS_ENFILE(s)         ((s) == GBW_ENFILE)
/** bad file # */
#define GBW_STATUS_IS_EBADF(s)          ((s) == GBW_EBADF)
/** invalid argument */
#define GBW_STATUS_IS_EINVAL(s)         ((s) == GBW_EINVAL)
/** illegal seek */
#define GBW_STATUS_IS_ESPIPE(s)         ((s) == GBW_ESPIPE)

/** operation would block */
#if !defined(EWOULDBLOCK) || !defined(EAGAIN)
#define GBW_STATUS_IS_EAGAIN(s)         ((s) == GBW_EAGAIN)
#elif (EWOULDBLOCK == EAGAIN)
#define GBW_STATUS_IS_EAGAIN(s)         ((s) == GBW_EAGAIN)
#else
#define GBW_STATUS_IS_EAGAIN(s)         ((s) == GBW_EAGAIN \
                                      || (s) == EWOULDBLOCK)
#endif

/** interrupted system call */
#define GBW_STATUS_IS_EINTR(s)          ((s) == GBW_EINTR)
/** socket operation on a non-socket */
#define GBW_STATUS_IS_ENOTSOCK(s)       ((s) == GBW_ENOTSOCK)
/** Connection Refused */
#define GBW_STATUS_IS_ECONNREFUSED(s)   ((s) == GBW_ECONNREFUSED)
/** operation now in progress */
#define GBW_STATUS_IS_EINPROGRESS(s)    ((s) == GBW_EINPROGRESS)

/**
 * Software caused connection abort
 * @remark
 * EPROTO on certain older kernels really means ECONNABORTED, so we need to
 * ignore it for them.  See discussion in new-httpd archives nh.9701 & nh.9603
 *
 * There is potentially a bug in Solaris 2.x x<6, and other boxes that
 * implement tcp sockets in userland (i.e. on top of STREAMS).  On these
 * systems, EPROTO can actually result in a fatal loop.  See PR#981 for
 * example.  It's hard to handle both uses of EPROTO.
 */
#ifdef EPROTO
#define GBW_STATUS_IS_ECONNABORTED(s)    ((s) == GBW_ECONNABORTED \
                                       || (s) == EPROTO)
#else
#define GBW_STATUS_IS_ECONNABORTED(s)    ((s) == GBW_ECONNABORTED)
#endif

/** Connection Reset by peer */
#define GBW_STATUS_IS_ECONNRESET(s)      ((s) == GBW_ECONNRESET)
/** Operation timed out
 *  @deprecated */
#define GBW_STATUS_IS_ETIMEDOUT(s)      ((s) == GBW_ETIMEDOUT)
/** no route to host */
#define GBW_STATUS_IS_EHOSTUNREACH(s)    ((s) == GBW_EHOSTUNREACH)
/** network is unreachable */
#define GBW_STATUS_IS_ENETUNREACH(s)     ((s) == GBW_ENETUNREACH)
/** inappropiate file type or format */
#define GBW_STATUS_IS_EFTYPE(s)          ((s) == GBW_EFTYPE)
/** broken pipe */
#define GBW_STATUS_IS_EPIPE(s)           ((s) == GBW_EPIPE)
/** cross device link */
#define GBW_STATUS_IS_EXDEV(s)           ((s) == GBW_EXDEV)
/** Directory Not Empty */
#define GBW_STATUS_IS_ENOTEMPTY(s)       ((s) == GBW_ENOTEMPTY || \
                                          (s) == GBW_EEXIST)
/** Address Family not supported */
#define GBW_STATUS_IS_EAFNOSUPPORT(s)    ((s) == GBW_EAFNOSUPPORT)
/** @} */


#endif /* GBW_ERRNO_H */
