/*
 *
 *      Filename: gbw_file_open.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 15:15:44
 * Last Modified: 2017-01-11 15:32:20
 */

#include "gbw_file.h"
#include "gbw_file_info.h"
#include "gbw_errno.h"

static int file_cleanup(gbw_file_t *file, int is_child)
{
    int rv = 0;
    int fd = file->filedes;

    /* Set file descriptor to -1 before close(), so that there is no
     * chance of returning an already closed FD from gbw_os_file_get().
     */
    file->filedes = -1;

    if (close(fd) == 0) {
        /* Only the parent process should delete the file! */
        if (!is_child && (file->flags & GBW_FOPEN_DELONCLOSE)) {
            unlink(file->fname);
        }
    }
    else {
        /* Restore, close() was not successful. */
        file->filedes = fd;

        /* Are there any error conditions other than EINTR or EBADF? */
        rv = errno;
    }
    return rv;
}

int gbw_unix_file_cleanup(void *thefile)
{
    gbw_file_t *file = thefile;
    int flush_rv = 0, rv = 0;

    if (file->buffered) {
        flush_rv = gbw_file_flush(file);
    }

    rv = file_cleanup(file, 0);

    return rv != 0 ? rv : flush_rv;
}

int gbw_unix_child_file_cleanup(void *thefile)
{
    return file_cleanup(thefile, 1);
}

int gbw_file_open(gbw_file_t **new, 
                                        const char *fname, 
                                        int32_t flag, 
                                        int32_t perm, 
                                        gbw_pool_t *pool)
{
    int fd;
    int oflags = 0;

    if ((flag & GBW_FOPEN_READ) && (flag & GBW_FOPEN_WRITE)) {
        oflags = O_RDWR;
    }
    else if (flag & GBW_FOPEN_READ) {
        oflags = O_RDONLY;
    }
    else if (flag & GBW_FOPEN_WRITE) {
        oflags = O_WRONLY;
    }
    else {
        return GBW_EACCES; 
    }

    if (flag & GBW_FOPEN_CREATE) {
        oflags |= O_CREAT;
        if (flag & GBW_FOPEN_EXCL) {
            oflags |= O_EXCL;
        }
    }
    if ((flag & GBW_FOPEN_EXCL) && !(flag & GBW_FOPEN_CREATE)) {
        return GBW_EACCES;
    }   

    if (flag & GBW_FOPEN_APPEND) {
        oflags |= O_APPEND;
    }
    if (flag & GBW_FOPEN_TRUNCATE) {
        oflags |= O_TRUNC;
    }
#ifdef O_BINARY
    if (flag & GBW_FOPEN_BINARY) {
        oflags |= O_BINARY;
    }
#endif

    if (flag & GBW_FOPEN_NONBLOCK) {
#ifdef O_NONBLOCK
        oflags |= O_NONBLOCK;
#else
        return GBW_ENOTIMPL;
#endif
    }

#ifdef O_CLOEXEC
    /* Introduced in Linux 2.6.23. Silently ignored on earlier Linux kernels.
     */
    if (!(flag & GBW_FOPEN_NOCLEANUP)) {
        oflags |= O_CLOEXEC;
}
#endif
 
#if GBW_HAS_LARGE_FILES && defined(_LARGEFILE64_SOURCE)
    oflags |= O_LARGEFILE;
#elif defined(O_LARGEFILE)
    if (flag & GBW_FOPEN_LARGEFILE) {
        oflags |= O_LARGEFILE;
    }
#endif

    if (perm == GBW_OS_DEFAULT) {
        fd = open(fname, oflags, 0666);
    }
    else {
        fd = open(fname, oflags, gbw_perms2mode(perm));
    } 
    if (fd < 0) {
       return errno;
    }
    if (!(flag & GBW_FOPEN_NOCLEANUP)) {
#ifdef O_CLOEXEC
        static int has_o_cloexec = 0;
        if (!has_o_cloexec)
#endif
        {
            int flags;

            if ((flags = fcntl(fd, F_GETFD)) == -1) {
                close(fd);
                return errno;
            }
            if ((flags & FD_CLOEXEC) == 0) {
                flags |= FD_CLOEXEC;
                if (fcntl(fd, F_SETFD, flags) == -1) {
                    close(fd);
                    return errno;
                }
            }
#ifdef O_CLOEXEC
            else {
                has_o_cloexec = 1;
            }
#endif
        }
    }

    (*new) = (gbw_file_t *)gbw_pcalloc(pool, sizeof(gbw_file_t));
    (*new)->pool = pool;
    (*new)->flags = flag;
    (*new)->filedes = fd;

    (*new)->fname = gbw_pstrdup(pool, fname);

    (*new)->blocking = BLK_ON;
    (*new)->buffered = (flag & GBW_FOPEN_BUFFERED) > 0;

    if ((*new)->buffered) {
        (*new)->buffer = gbw_palloc(pool, GBW_FILE_DEFAULT_BUFSIZE);
        (*new)->bufsize = GBW_FILE_DEFAULT_BUFSIZE;
    }
    else {
        (*new)->buffer = NULL;
    }

    (*new)->is_pipe = 0;
    (*new)->timeout = -1;
    (*new)->ungetchar = -1;
    (*new)->eof_hit = 0;
    (*new)->filePtr = 0;
    (*new)->bufpos = 0;
    (*new)->dataRead = 0;
    (*new)->direction = 0;
    return 0;
}

int gbw_file_close(gbw_file_t *file)
{
    return close(file->filedes);
}

int gbw_file_remove(const char *path, gbw_pool_t *pool)
{
	pool = pool;

    if (unlink(path) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}

int gbw_file_rename(const char *from_path, 
                                          const char *to_path,
                                          gbw_pool_t *p)
{
	p = p;

    if (rename(from_path, to_path) != 0) {
        return errno;
    }
    return 0;
}

int gbw_os_file_put(gbw_file_t **file, 
                                          int *thefile,
                                          int32_t flags, gbw_pool_t *pool)
{
    int *dafile = thefile;
    
    (*file) = gbw_pcalloc(pool, sizeof(gbw_file_t));
    (*file)->pool = pool;
    (*file)->eof_hit = 0;
    (*file)->blocking = BLK_UNKNOWN; /* in case it is a pipe */
    (*file)->timeout = -1;
    (*file)->ungetchar = -1; /* no char avail */
    (*file)->filedes = *dafile;
    (*file)->flags = flags | GBW_FOPEN_NOCLEANUP;
    (*file)->buffered = (flags & GBW_FOPEN_BUFFERED) > 0;

    if ((*file)->buffered) {
        (*file)->buffer = gbw_palloc(pool, GBW_FILE_DEFAULT_BUFSIZE);
        (*file)->bufsize = GBW_FILE_DEFAULT_BUFSIZE;
    }
    return 0;
}    

int gbw_file_eof(gbw_file_t *fptr)
{
    if (fptr->eof_hit == 1) {
        return GBW_EOF;
    }
    return 0;
}   

int gbw_file_open_flags_stderr(gbw_file_t **thefile, 
                                                     int32_t flags,
                                                     gbw_pool_t *pool)
{
    int fd = STDERR_FILENO;

    return gbw_os_file_put(thefile, &fd, flags | GBW_FOPEN_WRITE, pool);
}

int gbw_file_open_flags_stdout(gbw_file_t **thefile, 
                                                     int32_t flags,
                                                     gbw_pool_t *pool)
{
    int fd = STDOUT_FILENO;

    return gbw_os_file_put(thefile, &fd, flags | GBW_FOPEN_WRITE, pool);
}

int gbw_file_open_flags_stdin(gbw_file_t **thefile, 
                                                    int32_t flags,
                                                    gbw_pool_t *pool)
{
    int fd = STDIN_FILENO;

    return gbw_os_file_put(thefile, &fd, flags | GBW_FOPEN_READ, pool);
}

int gbw_file_open_stderr(gbw_file_t **thefile, 
                                               gbw_pool_t *pool)
{
    return gbw_file_open_flags_stderr(thefile, 0, pool);
}

int gbw_file_open_stdout(gbw_file_t **thefile, 
                                               gbw_pool_t *pool)
{
    return gbw_file_open_flags_stdout(thefile, 0, pool);
}

int gbw_file_open_stdin(gbw_file_t **thefile, 
                                              gbw_pool_t *pool)
{
    return gbw_file_open_flags_stdin(thefile, 0, pool);
}




int gbw_file_link(const char *from_path, 
                                          const char *to_path)
{
    if (link(from_path, to_path) == -1) {
        return errno;
    }

    return 0;
}
