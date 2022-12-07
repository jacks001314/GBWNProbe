/*shajf*/

#ifndef GBW_WALK_DIR_H
#define GBW_WALK_DIR_H

#include <dirent.h>
#include <sys/stat.h>

/* Some useful definitions */
#undef GBW_FALSE
#define GBW_FALSE   ((int) 0)
#undef GBW_TRUE
#define GBW_TRUE    ((int) 1)
#undef GBW_SKIP
#define GBW_SKIP	((int) 2)

enum {
	GBW_ACTION_RECURSE        = (1 << 0),
	GBW_ACTION_FOLLOWLINKS    = (1 << 1),
	GBW_ACTION_FOLLOWLINKS_L0 = (1 << 2),
	GBW_ACTION_DEPTHFIRST     = (1 << 3),
	GBW_ACTION_QUIET          = (1 << 5),
};

/* fileAction return value of 0 on any file in directory will make
 * gbw_walk_dir() return 0, but it doesn't stop directory traversal
 * (fileAction/dirAction will be called on each file).
 *
 * If !GBW_ACTION_RECURSE, dirAction is called on the directory and its
 * return value is returned from gbw_walk_dir(). No recursion.
 *
 * If GBW_ACTION_RECURSE, gbw_walk_dir() is called on each directory.
 * If any one of these calls returns 0, current gbw_walk_dir() returns 0.
 *
 * If GBW_ACTION_DEPTHFIRST, dirAction is called after recurse.
 * If it returns 0, the warning is printed and gbw_walk_dir() returns 0.
 *
 * If !GBW_ACTION_DEPTHFIRST, dirAction is called before we recurse.
 * Return value of 0 (FALSE) or 2 (SKIP) prevents recursion
 * into that directory, instead gbw_walk_dir() returns 0 (if FALSE)
 * or 1 (if SKIP)
 *
 * followLinks=0/1 differs mainly in handling of links to dirs.
 * 0: lstat(statbuf). Calls fileAction on link name even if points to dir.
 * 1: stat(statbuf). Calls dirAction and optionally recurse on link to dir.
 */

int gbw_walk_dir(const char *fileName,
		unsigned flags,
		int  (*fileAction)(const char *fileName, struct stat *statbuf, void* userData, int depth),
		int  (*dirAction)(const char *fileName, struct stat *statbuf, void* userData, int depth),
		int  (*acceptAction)(const char *fileName),
        void* userData,
		unsigned depth);

#endif /*GBW_WALK_DIR_H*/
