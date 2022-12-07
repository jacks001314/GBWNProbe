/*
 *
 *      Filename: gbw_filestat.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 14:20:18
 * Last Modified: 2017-01-11 15:10:33
 */

#include "gbw_file_info.h"
#include "gbw_file.h"
#include "gbw_errno.h"
#include "gbw_time.h"

static gbw_filetype_e filetype_from_mode(mode_t mode)
{
    gbw_filetype_e type;

    switch (mode & S_IFMT) {
    case S_IFREG:
        type = GBW_REG;  break;
    case S_IFDIR:
        type = GBW_DIR;  break;
    case S_IFLNK:
        type = GBW_LNK;  break;
    case S_IFCHR:
        type = GBW_CHR;  break;
    case S_IFBLK:
        type = GBW_BLK;  break;
#if defined(S_IFFIFO)
    case S_IFFIFO:
        type = GBW_PIPE; break;
#endif
#if !defined(BEOS) && defined(S_IFSOCK)
    case S_IFSOCK:
        type = GBW_SOCK; break;
#endif

    default:
	/* Work around missing S_IFxxx values above
         * for Linux et al.
         */
#if !defined(S_IFFIFO) && defined(S_ISFIFO)
    	if (S_ISFIFO(mode)) {
            type = GBW_PIPE;
	} else
#endif
#if !defined(BEOS) && !defined(S_IFSOCK) && defined(S_ISSOCK)
    	if (S_ISSOCK(mode)) {
            type = GBW_SOCK;
	} else
#endif
        type = GBW_UNKFILE;
    }
    return type;
}

static void fill_out_finfo(gbw_finfo_t *finfo, struct_stat *info,
                           int32_t wanted)
{
	wanted = wanted;

    finfo->valid = GBW_FINFO_MIN | GBW_FINFO_IDENT | GBW_FINFO_NLINK
                 | GBW_FINFO_OWNER | GBW_FINFO_PROT;
    finfo->protection = gbw_mode2perms(info->st_mode);
    finfo->filetype = filetype_from_mode(info->st_mode);
    finfo->user = info->st_uid;
    finfo->group = info->st_gid;
    finfo->size = info->st_size;
    finfo->device = info->st_dev;
    finfo->nlink = info->st_nlink;

    /* Check for overflow if storing a 64-bit st_ino in a 32-bit
     * ino_t for LFS builds: */
    if (sizeof(ino_t) >= sizeof(info->st_ino)
        || (ino_t)info->st_ino == info->st_ino) {
        finfo->inode = info->st_ino;
    } else {
        finfo->valid &= ~GBW_FINFO_INODE;
    }

    gbw_time_ansi_put(&finfo->atime, info->st_atime);
    finfo->atime += info->st_atim.tv_nsec / 1000L;

    gbw_time_ansi_put(&finfo->mtime, info->st_mtime);
    finfo->mtime += info->st_mtim.tv_nsec / 1000L;

    gbw_time_ansi_put(&finfo->ctime, info->st_ctime);
    finfo->ctime += info->st_ctim.tv_nsec / 1000L;

#ifdef HAVE_STRUCT_STAT_ST_BLOCKS
#ifdef DEV_BSIZE
    finfo->csize = (gbw_off_t)info->st_blocks * (gbw_off_t)DEV_BSIZE;
#else
    finfo->csize = (gbw_off_t)info->st_blocks * (gbw_off_t)512;
#endif
    finfo->valid |= GBW_FINFO_CSIZE;
#endif
}

int gbw_file_info_get_locked(gbw_finfo_t *finfo, int32_t wanted,
                                      gbw_file_t *thefile)
{
    struct_stat info;

    if (thefile->buffered) {
        int rv = gbw_file_flush_locked(thefile);
        if (rv != 0)
            return rv;
    }

    if (fstat(thefile->filedes, &info) == 0) {
        finfo->pool = thefile->pool;
        finfo->fname = thefile->fname;
        fill_out_finfo(finfo, &info, wanted);
        return (wanted & ~finfo->valid) ? GBW_INCOMPLETE : 0;
    }
    else {
        return errno;
    }
}

int gbw_file_info_get(gbw_finfo_t *finfo, 
                                            int32_t wanted,
                                            gbw_file_t *thefile)
{
    struct_stat info;

    if (thefile->buffered) {
        int rv = gbw_file_flush(thefile);
        if (rv != 0)
            return rv;
    }

    if (fstat(thefile->filedes, &info) == 0) {
        finfo->pool = thefile->pool;
        finfo->fname = thefile->fname;
        fill_out_finfo(finfo, &info, wanted);
        return (wanted & ~finfo->valid) ? GBW_INCOMPLETE : 0;
    }
    else {
        return errno;
    }
}

int gbw_file_perms_set(const char *fname, 
                                             int32_t perms)
{
    mode_t mode = gbw_perms2mode(perms);

    if (chmod(fname, mode) == -1)
        return errno;
    return 0;
}

int gbw_file_attrs_set(const char *fname,
                                             gbw_fileattrs_t attributes,
                                             gbw_fileattrs_t attr_mask,
                                             gbw_pool_t *pool)
{
    int status;
    gbw_finfo_t finfo;

    /* Don't do anything if we can't handle the requested attributes */
    if (!(attr_mask & (GBW_FILE_ATTR_READONLY
                       | GBW_FILE_ATTR_EXECUTABLE)))
        return 0;

    status = gbw_stat(&finfo, fname, GBW_FINFO_PROT, pool);
    if (status)
        return status;

    /* ### TODO: should added bits be umask'd? */
    if (attr_mask & GBW_FILE_ATTR_READONLY)
    {
        if (attributes & GBW_FILE_ATTR_READONLY)
        {
            finfo.protection &= ~GBW_UWRITE;
            finfo.protection &= ~GBW_GWRITE;
            finfo.protection &= ~GBW_WWRITE;
        }
        else
        {
            /* ### umask this! */
            finfo.protection |= GBW_UWRITE;
            finfo.protection |= GBW_GWRITE;
            finfo.protection |= GBW_WWRITE;
        }
    }

    if (attr_mask & GBW_FILE_ATTR_EXECUTABLE)
    {
        if (attributes & GBW_FILE_ATTR_EXECUTABLE)
        {
            /* ### umask this! */
            finfo.protection |= GBW_UEXECUTE;
            finfo.protection |= GBW_GEXECUTE;
            finfo.protection |= GBW_WEXECUTE;
        }
        else
        {
            finfo.protection &= ~GBW_UEXECUTE;
            finfo.protection &= ~GBW_GEXECUTE;
            finfo.protection &= ~GBW_WEXECUTE;
        }
    }

    return gbw_file_perms_set(fname, finfo.protection);
}


int gbw_file_mtime_set(const char *fname,
                                              gbw_time_t mtime,
                                              gbw_pool_t *pool)
{
    int status;
    gbw_finfo_t finfo;

    status = gbw_stat(&finfo, fname, GBW_FINFO_ATIME, pool);
    if (status) {
        return status;
    }

    {
      struct timeval tvp[2];
    
      tvp[0].tv_sec = gbw_time_sec(finfo.atime);
      tvp[0].tv_usec = gbw_time_usec(finfo.atime);
      tvp[1].tv_sec = gbw_time_sec(mtime);
      tvp[1].tv_usec = gbw_time_usec(mtime);
      
      if (utimes(fname, tvp) == -1) {
        return errno;
      }
    }

    return 0;
}


int gbw_stat(gbw_finfo_t *finfo, 
                                   const char *fname, 
                                   int32_t wanted, gbw_pool_t *pool)
{
    struct_stat info;
    int srv;

    if (wanted & GBW_FINFO_LINK)
        srv = lstat(fname, &info);
    else
        srv = stat(fname, &info);

    if (srv == 0) {
        finfo->pool = pool;
        finfo->fname = fname;
        fill_out_finfo(finfo, &info, wanted);
        if (wanted & GBW_FINFO_LINK)
            wanted &= ~GBW_FINFO_LINK;
        return (wanted & ~finfo->valid) ? GBW_INCOMPLETE : 0;
    }
    else {
#if !defined(ENOENT) || !defined(ENOTDIR)
#error ENOENT || ENOTDIR not defined; please see the
#error comments at this line in the source for a workaround.
        /*
         * If ENOENT || ENOTDIR is not defined in one of the your OS's
         * include files, GW cannot report a good reason why the stat()
         * of the file failed; there are cases where it can fail even though
         * the file exists.  This opens holes in Apache, for example, because
         * it becomes possible for someone to get a directory listing of a 
         * directory even though there is an index (eg. index.html) file in 
         * it.  If you do not have a problem with this, delete the above 
         * #error lines and start the compile again.  If you need to do this,
         * please submit a bug report to http://www.apache.org/bug_report.html
         * letting us know that you needed to do this.  Please be sure to 
         * include the operating system you are using.
         */
        /* WARNING: All errors will be handled as not found
         */
#if !defined(ENOENT) 
        return GBW_ENOENT;
#else
        /* WARNING: All errors but not found will be handled as not directory
         */
        if (errno != ENOENT)
            return GBW_ENOENT;
        else
            return errno;
#endif
#else /* All was defined well, report the usual: */
        return errno;
#endif
    }
}


