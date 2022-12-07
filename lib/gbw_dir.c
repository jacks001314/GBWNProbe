/*
 *
 *      Filename: gbw_dir.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 12:39:57
 * Last Modified: 2017-01-11 13:08:59
 */

#include "gbw_file_info.h"
#include "gbw_errno.h"
#include "gbw_constants.h"
#include "gbw_string.h"
#include "gbw_file.h"

#define PATH_SEPARATOR '/'

/* Remove trailing separators that don't affect the meaning of PATH. */
static const char *path_canonicalize (const char *path, gbw_pool_t *pool)
{
    /* At some point this could eliminate redundant components.  For
     * now, it just makes sure there is no trailing slash. */
    size_t len = strlen (path);
    size_t orig_len = len;
    
    while ((len > 0) && (path[len - 1] == PATH_SEPARATOR))
        len--;
    
    if (len != orig_len)
        return gbw_pstrndup (pool, path, len);
    else
        return path;
}

/* Remove one component off the end of PATH. */
static char *path_remove_last_component (const char *path, gbw_pool_t *pool)
{
    const char *newpath = path_canonicalize (path, pool);
    int i;
    
    for (i = (strlen(newpath) - 1); i >= 0; i--) {
        if (path[i] == PATH_SEPARATOR)
            break;
    }

    return gbw_pstrndup (pool, path, (i < 0) ? 0 : i);
}

int gbw_dir_open(gbw_dir_t **new, const char *dirname, 
                          gbw_pool_t *pool)
{
    /* On some platforms (e.g., Linux+GNU libc), d_name[] in struct 
     * dirent is declared with enough storage for the name.  On other
     * platforms (e.g., Solaris 8 for Intel), d_name is declared as a
     * one-byte array.  Note: gcc evaluates this at compile time.
     */
    size_t dirent_size = 
        sizeof(*(*new)->entry) +
        (sizeof((*new)->entry->d_name) > 1 ? 0 : 255);
    DIR *dir = opendir(dirname);

    if (!dir) {
        return errno;
    }

    (*new) = (gbw_dir_t *)gbw_palloc(pool, sizeof(gbw_dir_t));

    (*new)->pool = pool;
    (*new)->dirname = gbw_pstrdup(pool, dirname);
    (*new)->dirstruct = dir;
    (*new)->entry = gbw_pcalloc(pool, dirent_size);

    return 0;
}

int gbw_dir_close(gbw_dir_t *dir)
{
    if (closedir(dir->dirstruct) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}

#ifdef DIRENT_TYPE
static gbw_filetype_e filetype_from_dirent_type(int type)
{
    switch (type) {
    case DT_REG:
        return GBW_REG;
    case DT_DIR:
        return GBW_DIR;
    case DT_LNK:
        return GBW_LNK;
    case DT_CHR:
        return GBW_CHR;
    case DT_BLK:
        return GBW_BLK;
#if defined(DT_FIFO)
    case DT_FIFO:
        return GBW_PIPE;
#endif
#if !defined(BEOS) && defined(DT_SOCK)
    case DT_SOCK:
        return GBW_SOCK;
#endif
    default:
        return GBW_UNKFILE;
    }
}
#endif

int gbw_dir_read(gbw_finfo_t *finfo, int32_t wanted,
                          gbw_dir_t *thedir)
{
    int ret = 0;
#ifdef DIRENT_TYPE
    gbw_filetype_e type;
#endif



    /* We're about to call a non-thread-safe readdir() that may
       possibly set `errno', and the logic below actually cares about
       errno after the call.  Therefore we need to clear errno first. */
    errno = 0;
    thedir->entry = readdir(thedir->dirstruct);
    if (thedir->entry == NULL) {
        /* If NULL was returned, this can NEVER be a success. Can it?! */
        if (errno == 0) {
            ret = GBW_ENOENT;
        }
        else
            ret = errno;
    }

    /* No valid bit flag to test here - do we want one? */
    finfo->fname = NULL;

    if (ret) {
        finfo->valid = 0;
        return ret;
    }

#ifdef DIRENT_TYPE
    type = filetype_from_dirent_type(thedir->entry->DIRENT_TYPE);
    if (type != GBW_UNKFILE) {
        wanted &= ~GBW_FINFO_TYPE;
    }
#endif
#ifdef DIRENT_INODE
    if (thedir->entry->DIRENT_INODE && thedir->entry->DIRENT_INODE != -1) {

        wanted &= ~GBW_FINFO_INODE;
    }
#endif /* DIRENT_INODE */

    wanted &= ~GBW_FINFO_NAME;

    if (wanted)
    {
        char fspec[GBW_PATH_MAX];
        char *end;

        end = gbw_cpystrn(fspec, thedir->dirname, sizeof fspec);

        if (end > fspec && end[-1] != '/' && (end < fspec + GBW_PATH_MAX))
            *end++ = '/';

        gbw_cpystrn(end, thedir->entry->d_name, 
                    sizeof fspec - (end - fspec));

        ret = gbw_stat(finfo, fspec, GBW_FINFO_LINK | wanted, thedir->pool);
        /* We passed a stack name that will disappear */
        finfo->fname = NULL;
    }

    if (wanted && (ret == 0 || ret == GBW_INCOMPLETE)) {
        wanted &= ~finfo->valid;
    }
    else {
        /* We don't bail because we fail to stat, when we are only -required-
         * to readdir... but the result will be GBW_INCOMPLETE
         */
        finfo->pool = thedir->pool;
        finfo->valid = 0;
#ifdef DIRENT_TYPE
        if (type != GBW_UNKFILE) {
            finfo->filetype = type;
            finfo->valid |= GBW_FINFO_TYPE;
        }
#endif
#ifdef DIRENT_INODE
        if (thedir->entry->DIRENT_INODE && thedir->entry->DIRENT_INODE != -1) {
            finfo->inode = thedir->entry->DIRENT_INODE;
            finfo->valid |= GBW_FINFO_INODE;
        }
#endif
    }

    finfo->name = gbw_pstrdup(thedir->pool, thedir->entry->d_name);
    finfo->valid |= GBW_FINFO_NAME;

    if (wanted)
        return GBW_INCOMPLETE;

    return 0;
}

int gbw_dir_rewind(gbw_dir_t *thedir)
{
    rewinddir(thedir->dirstruct);
    return 0;
}

int gbw_dir_make(const char *path, int32_t perm, 
                          gbw_pool_t *pool)
{
	pool = pool;

    mode_t mode = gbw_perms2mode(perm);

    if (mkdir(path, mode) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}

int gbw_dir_make_recursive(const char *path, int32_t perm,
                                           gbw_pool_t *pool) 
{
    int gbw_err = 0;

    gbw_err = gbw_dir_make (path, perm, pool); /* Try to make PATH right out */
    
    if (gbw_err == ENOENT) { /* Missing an intermediate dir */
        char *dir;
        
        dir = path_remove_last_component(path, pool);
        /* If there is no path left, give up. */
        if (dir[0] == '\0') {
            return gbw_err;
        }

        gbw_err = gbw_dir_make_recursive(dir, perm, pool);
        
        if (!gbw_err) 
            gbw_err = gbw_dir_make (path, perm, pool);
    }

    /*
     * It's OK if PATH exists. Timing issues can lead to the second
     * gbw_dir_make being called on existing dir, therefore this check
     * has to come last.
     */
    if (GBW_STATUS_IS_EEXIST(gbw_err))
        return 0;

    return gbw_err;
}

int gbw_dir_remove(const char *path, gbw_pool_t *pool)
{
	pool = pool;

    if (rmdir(path) == 0) {
        return 0;
    }
    else {
        return errno;
    }
}



  
