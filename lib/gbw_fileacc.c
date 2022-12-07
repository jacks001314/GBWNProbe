/*
 *
 *      Filename: gbw_fileacc.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 13:10:06
 * Last Modified: 2017-01-11 13:18:40
 */

#include "gbw_file_info.h"

mode_t gbw_perms2mode(int32_t perms)
{
    mode_t mode = 0;

    if (perms & GBW_USETID)
        mode |= S_ISUID;
    if (perms & GBW_UREAD)
        mode |= S_IRUSR;
    if (perms & GBW_UWRITE)
        mode |= S_IWUSR;
    if (perms & GBW_UEXECUTE)
        mode |= S_IXUSR;

    if (perms & GBW_GSETID)
        mode |= S_ISGID;
    if (perms & GBW_GREAD)
        mode |= S_IRGRP;
    if (perms & GBW_GWRITE)
        mode |= S_IWGRP;
    if (perms & GBW_GEXECUTE)
        mode |= S_IXGRP;

#ifdef S_ISVTX
    if (perms & GBW_WSTICKY)
        mode |= S_ISVTX;
#endif
    if (perms & GBW_WREAD)
        mode |= S_IROTH;
    if (perms & GBW_WWRITE)
        mode |= S_IWOTH;
    if (perms & GBW_WEXECUTE)
        mode |= S_IXOTH;

    return mode;
}

int32_t gbw_mode2perms(mode_t mode)
{
    int32_t perms = 0;

    if (mode & S_ISUID)
        perms |= GBW_USETID;
    if (mode & S_IRUSR)
        perms |= GBW_UREAD;
    if (mode & S_IWUSR)
        perms |= GBW_UWRITE;
    if (mode & S_IXUSR)
        perms |= GBW_UEXECUTE;

    if (mode & S_ISGID)
        perms |= GBW_GSETID;
    if (mode & S_IRGRP)
        perms |= GBW_GREAD;
    if (mode & S_IWGRP)
        perms |= GBW_GWRITE;
    if (mode & S_IXGRP)
        perms |= GBW_GEXECUTE;

#ifdef S_ISVTX
    if (mode & S_ISVTX)
        perms |= GBW_WSTICKY;
#endif
    if (mode & S_IROTH)
        perms |= GBW_WREAD;
    if (mode & S_IWOTH)
        perms |= GBW_WWRITE;
    if (mode & S_IXOTH)
        perms |= GBW_WEXECUTE;

    return perms;
}


