/*
 *
 *      Filename: gbw_file_info.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 11:10:18
 * Last Modified: 2017-01-11 11:10:18
 */

#ifndef GBW_FILE_INFO_H
#define GBW_FILE_INFO_H

#include "gbw_tables.h"
/**
 * @defgroup gbw_file_info File Information
 * @ingroup GW 
 * @{
 */

/* Many applications use the type member to determine the
 * existance of a file or initialization of the file info,
 * so the GBW_NOFILE value must be distinct from GBW_UNKFILE.
 */

/** gbw_filetype_e values for the filetype member of the 
 * gbw_file_info_t structure
 * @warning: Not all of the filetypes below can be determined.
 * For example, a given platform might not correctly report 
 * a socket descriptor as GBW_SOCK if that type isn't 
 * well-identified on that platform.  In such cases where
 * a filetype exists but cannot be described by the recognized
 * flags below, the filetype will be GBW_UNKFILE.  If the
 * filetype member is not determined, the type will be GBW_NOFILE.
 */

typedef enum {
    GBW_NOFILE = 0,     /**< no file type determined */
    GBW_REG,            /**< a regular file */
    GBW_DIR,            /**< a directory */
    GBW_CHR,            /**< a character device */
    GBW_BLK,            /**< a block device */
    GBW_PIPE,           /**< a FIFO / pipe */
    GBW_LNK,            /**< a symbolic link */
    GBW_SOCK,           /**< a [unix domain] socket */
    GBW_UNKFILE = 127   /**< a file of some other unknown type */
} gbw_filetype_e; 

#define GBW_PATH_MAX 8192
/**
 * @defgroup gbw_file_permissions File Permissions flags 
 * @{
 */

#define GBW_FPROT_USETID      0x8000 /**< Set user id */
#define GBW_FPROT_UREAD       0x0400 /**< Read by user */
#define GBW_FPROT_UWRITE      0x0200 /**< Write by user */
#define GBW_FPROT_UEXECUTE    0x0100 /**< Execute by user */

#define GBW_FPROT_GSETID      0x4000 /**< Set group id */
#define GBW_FPROT_GREAD       0x0040 /**< Read by group */
#define GBW_FPROT_GWRITE      0x0020 /**< Write by group */
#define GBW_FPROT_GEXECUTE    0x0010 /**< Execute by group */

#define GBW_FPROT_WSTICKY     0x2000 /**< Sticky bit */
#define GBW_FPROT_WREAD       0x0004 /**< Read by others */
#define GBW_FPROT_WWRITE      0x0002 /**< Write by others */
#define GBW_FPROT_WEXECUTE    0x0001 /**< Execute by others */

#define GBW_FPROT_OS_DEFAULT  0x0FFF /**< use OS's default permissions */

/* additional permission flags for gbw_file_copy  and gbw_file_append */
#define GBW_FPROT_FILE_SOURCE_PERMS 0x1000 /**< Copy source file's permissions */
    
/* backcompat */
#define GBW_USETID     GBW_FPROT_USETID     /**< @deprecated @see GBW_FPROT_USETID     */
#define GBW_UREAD      GBW_FPROT_UREAD      /**< @deprecated @see GBW_FPROT_UREAD      */
#define GBW_UWRITE     GBW_FPROT_UWRITE     /**< @deprecated @see GBW_FPROT_UWRITE     */
#define GBW_UEXECUTE   GBW_FPROT_UEXECUTE   /**< @deprecated @see GBW_FPROT_UEXECUTE   */
#define GBW_GSETID     GBW_FPROT_GSETID     /**< @deprecated @see GBW_FPROT_GSETID     */
#define GBW_GREAD      GBW_FPROT_GREAD      /**< @deprecated @see GBW_FPROT_GREAD      */
#define GBW_GWRITE     GBW_FPROT_GWRITE     /**< @deprecated @see GBW_FPROT_GWRITE     */
#define GBW_GEXECUTE   GBW_FPROT_GEXECUTE   /**< @deprecated @see GBW_FPROT_GEXECUTE   */
#define GBW_WSTICKY    GBW_FPROT_WSTICKY    /**< @deprecated @see GBW_FPROT_WSTICKY    */
#define GBW_WREAD      GBW_FPROT_WREAD      /**< @deprecated @see GBW_FPROT_WREAD      */
#define GBW_WWRITE     GBW_FPROT_WWRITE     /**< @deprecated @see GBW_FPROT_WWRITE     */
#define GBW_WEXECUTE   GBW_FPROT_WEXECUTE   /**< @deprecated @see GBW_FPROT_WEXECUTE   */
#define GBW_OS_DEFAULT GBW_FPROT_OS_DEFAULT /**< @deprecated @see GBW_FPROT_OS_DEFAULT */
#define GBW_FILE_SOURCE_PERMS GBW_FPROT_FILE_SOURCE_PERMS /**< @deprecated @see GBW_FPROT_FILE_SOURCE_PERMS */
    
/** @} */


/**
 * Structure for referencing directories.
 */
typedef struct gbw_dir_t          gbw_dir_t;
/**
 * Structure for determining file permissions.
 */

/**
 * @defgroup gbw_file_stat Stat Functions
 * @{
 */
/** file info structure */
typedef struct gbw_finfo_t        gbw_finfo_t;

#define GBW_FINFO_LINK   0x00000001 /**< Stat the link not the file itself if it is a link */
#define GBW_FINFO_MTIME  0x00000010 /**< Modification Time */
#define GBW_FINFO_CTIME  0x00000020 /**< Creation or inode-changed time */
#define GBW_FINFO_ATIME  0x00000040 /**< Access Time */
#define GBW_FINFO_SIZE   0x00000100 /**< Size of the file */
#define GBW_FINFO_CSIZE  0x00000200 /**< Storage size consumed by the file */
#define GBW_FINFO_DEV    0x00001000 /**< Device */
#define GBW_FINFO_INODE  0x00002000 /**< Inode */
#define GBW_FINFO_NLINK  0x00004000 /**< Number of links */
#define GBW_FINFO_TYPE   0x00008000 /**< Type */
#define GBW_FINFO_USER   0x00010000 /**< User */
#define GBW_FINFO_GROUP  0x00020000 /**< Group */
#define GBW_FINFO_UPROT  0x00100000 /**< User protection bits */
#define GBW_FINFO_GPROT  0x00200000 /**< Group protection bits */
#define GBW_FINFO_WPROT  0x00400000 /**< World protection bits */
#define GBW_FINFO_ICASE  0x01000000 /**< if dev is case insensitive */
#define GBW_FINFO_NAME   0x02000000 /**< ->name in proper case */

#define GBW_FINFO_MIN    0x00008170 /**< type, mtime, ctime, atime, size */
#define GBW_FINFO_IDENT  0x00003000 /**< dev and inode */
#define GBW_FINFO_OWNER  0x00030000 /**< user and group */
#define GBW_FINFO_PROT   0x00700000 /**<  all protections */
#define GBW_FINFO_NORM   0x0073b170 /**<  an atomic unix gbw_stat() */
#define GBW_FINFO_DIRENT 0x02000000 /**<  an atomic unix gbw_dir_read() */

/**
 * The file information structure.  This is analogous to the POSIX
 * stat structure.
 */
struct gbw_finfo_t {
    /** Allocates memory and closes lingering handles in the specified pool */
    gbw_pool_t *pool;
    /** The bitmask describing valid fields of this gbw_finfo_t structure 
     *  including all available 'wanted' fields and potentially more */
    int32_t valid;
    /** The access permissions of the file.  Mimics Unix access rights. */
    int32_t protection;
    /** The type of file.  One of GBW_REG, GBW_DIR, GBW_CHR, GBW_BLK, GBW_PIPE, 
     * GBW_LNK or GBW_SOCK.  If the type is undetermined, the value is GBW_NOFILE.
     * If the type cannot be determined, the value is GBW_UNKFILE.
     */
    gbw_filetype_e filetype;
    /** The user id that owns the file */
    uid_t user;
    /** The group id that owns the file */
    gid_t group;
    /** The inode of the file. */
    ino_t inode;
    /** The id of the device the file is on. */
    dev_t device;
    /** The number of hard links to the file. */
    int32_t nlink;
    /** The size of the file */
    off_t size;
    /** The storage size consumed by the file */
    off_t csize;
    /** The time the file was last accessed */
    int64_t atime;
    /** The time the file was last modified */
    int64_t mtime;
    /** The time the file was created, or the inode was last changed */
    int64_t ctime;
    /** The pathname of the file (possibly unrooted) */
    const char *fname;
    /** The file's name (no path) in filesystem case */
    const char *name;
    /** Unused */
    struct gbw_file_t *filehand;
};

struct gbw_dir_t {                                                                                                              
     gbw_pool_t *pool;                                                                                                           
     char *dirname;                                                                                                              
     DIR *dirstruct;                                                                                                             
     struct dirent *entry;                                                                                                       
 };     

/**
 * get the specified file's stats.  The file is specified by filename, 
 * instead of using a pre-opened file.
 * @param finfo Where to store the information about the file, which is
 * never touched if the call fails.
 * @param fname The name of the file to stat.
 * @param wanted The desired gbw_finfo_t fields, as a bit flag of GBW_FINFO_
                 values 
 * @param pool the pool to use to allocate the new file. 
 *
 * @note If @c GBW_INCOMPLETE is returned all the fields in @a finfo may
 *       not be filled in, and you need to check the @c finfo->valid bitmask
 *       to verify that what you're looking for is there.
 */ 
int gbw_stat(gbw_finfo_t *finfo, const char *fname,
                                   int32_t wanted, gbw_pool_t *pool);

/** @} */
/**
 * @defgroup gbw_dir Directory Manipulation Functions
 * @{
 */

/**
 * Open the specified directory.
 * @param new_dir The opened directory descriptor.
 * @param dirname The full path to the directory (use / on all systems)
 * @param pool The pool to use.
 */                        
int gbw_dir_open(gbw_dir_t **new_dir, 
                                       const char *dirname, 
                                       gbw_pool_t *pool);

/**
 * close the specified directory. 
 * @param thedir the directory descriptor to close.
 */                        
int gbw_dir_close(gbw_dir_t *thedir);

/**
 * Read the next entry from the specified directory. 
 * @param finfo the file info structure and filled in by gbw_dir_read
 * @param wanted The desired gbw_finfo_t fields, as a bit flag of GBW_FINFO_
                 values 
 * @param thedir the directory descriptor returned from gbw_dir_open
 * @remark No ordering is guaranteed for the entries read.
 *
 * @note If @c GBW_INCOMPLETE is returned all the fields in @a finfo may
 *       not be filled in, and you need to check the @c finfo->valid bitmask
 *       to verify that what you're looking for is there. When no more
 *       entries are available, GBW_ENOENT is returned.
 */                        
int gbw_dir_read(gbw_finfo_t *finfo, int32_t wanted,
                                       gbw_dir_t *thedir);

/**
 * Rewind the directory to the first entry.
 * @param thedir the directory descriptor to rewind.
 */                        
int gbw_dir_rewind(gbw_dir_t *thedir);
/** @} */

/**
 * @defgroup gbw_filepath Filepath Manipulation Functions
 * @{
 */

/** Cause gbw_filepath_merge to fail if addpath is above rootpath 
 * @bug in GW 0.9 and 1.x, this flag's behavior is undefined
 * if the rootpath is NULL or empty.  In GW 2.0 this should be
 * changed to imply NOTABSOLUTE if the rootpath is NULL or empty.
 */
#define GBW_FILEPATH_NOTABOVEROOT   0x01

/** internal: Only meaningful with GBW_FILEPATH_NOTABOVEROOT */
#define GBW_FILEPATH_SECUREROOTTEST 0x02

/** Cause gbw_filepath_merge to fail if addpath is above rootpath,
 * even given a rootpath /foo/bar and an addpath ../bar/bash
 */
#define GBW_FILEPATH_SECUREROOT     0x03

/** Fail gbw_filepath_merge if the merged path is relative */
#define GBW_FILEPATH_NOTRELATIVE    0x04

/** Fail gbw_filepath_merge if the merged path is absolute */
#define GBW_FILEPATH_NOTABSOLUTE    0x08

/** Return the file system's native path format (e.g. path delimiters
 * of ':' on MacOS9, '\' on Win32, etc.) */
#define GBW_FILEPATH_NATIVE         0x10

/** Resolve the true case of existing directories and file elements
 * of addpath, (resolving any aliases on Win32) and append a proper 
 * trailing slash if a directory
 */
#define GBW_FILEPATH_TRUENAME       0x20

/**
 * Extract the rootpath from the given filepath
 * @param rootpath the root file path returned with GBW_SUCCESS or GBW_EINCOMPLETE
 * @param filepath the pathname to parse for its root component
 * @param flags the desired rules to apply, from
 * <PRE>
 *      GBW_FILEPATH_NATIVE    Use native path separators (e.g. '\' on Win32)
 *      GBW_FILEPATH_TRUENAME  Tests that the root exists, and makes it proper
 * </PRE>
 * @param p the pool to allocate the new path string from
 * @remark on return, filepath points to the first non-root character in the
 * given filepath.  In the simplest example, given a filepath of "/foo", 
 * returns the rootpath of "/" and filepath points at "foo".  This is far 
 * more complex on other platforms, which will canonicalize the root form
 * to a consistant format, given the GBW_FILEPATH_TRUENAME flag, and also
 * test for the validity of that root (e.g., that a drive d:/ or network 
 * share //machine/foovol/). 
 * The function returns GBW_ERELATIVE if filepath isn't rooted (an
 * error), GBW_EINCOMPLETE if the root path is ambiguous (but potentially
 * legitimate, e.g. "/" on Windows is incomplete because it doesn't specify
 * the drive letter), or GBW_EBADPATH if the root is simply invalid.
 * GBW_SUCCESS is returned if filepath is an absolute path.
 */
int gbw_filepath_root(const char **rootpath, 
                                            const char **filepath, 
                                            int32_t flags,
                                            gbw_pool_t *p);

/**
 * Merge additional file path onto the previously processed rootpath
 * @param newpath the merged paths returned
 * @param rootpath the root file path (NULL uses the current working path)
 * @param addpath the path to add to the root path
 * @param flags the desired GBW_FILEPATH_ rules to apply when merging
 * @param p the pool to allocate the new path string from
 * @remark if the flag GBW_FILEPATH_TRUENAME is given, and the addpath 
 * contains wildcard characters ('*', '?') on platforms that don't support 
 * such characters within filenames, the paths will be merged, but the 
 * result code will be GBW_EPATHWILD, and all further segments will not
 * reflect the true filenames including the wildcard and following segments.
 */                        
int gbw_filepath_merge(char **newpath, 
                                             const char *rootpath,
                                             const char *addpath, 
                                             int32_t flags,
                                             gbw_pool_t *p);

/**
 * Split a search path into separate components
 * @param pathelts the returned components of the search path
 * @param liststr the search path (e.g., <tt>getenv("PATH")</tt>)
 * @param p the pool to allocate the array and path components from
 * @remark empty path components do not become part of @a pathelts.
 * @remark the path separator in @a liststr is system specific;
 * e.g., ':' on Unix, ';' on Windows, etc.
 */
int gbw_filepath_list_split(gbw_array_header_t **pathelts,
                                                  const char *liststr,
                                                  gbw_pool_t *p);

/**
 * Merge a list of search path components into a single search path
 * @param liststr the returned search path; may be NULL if @a pathelts is empty
 * @param pathelts the components of the search path
 * @param p the pool to allocate the search path from
 * @remark emtpy strings in the source array are ignored.
 * @remark the path separator in @a liststr is system specific;
 * e.g., ':' on Unix, ';' on Windows, etc.
 */
int gbw_filepath_list_merge(char **liststr,
                                                  gbw_array_header_t *pathelts,
                                                  gbw_pool_t *p);

/**
 * Return the default file path (for relative file names)
 * @param path the default path string returned
 * @param flags optional flag GBW_FILEPATH_NATIVE to retrieve the
 *              default file path in os-native format.
 * @param p the pool to allocate the default path string from
 */
int gbw_filepath_get(char **path, int32_t flags,
                                           gbw_pool_t *p);

/**
 * Set the default file path (for relative file names)
 * @param path the default path returned
 * @param p the pool to allocate any working storage
 */
int gbw_filepath_set(const char *path, gbw_pool_t *p);

/** The FilePath character encoding is unknown */
#define GBW_FILEPATH_ENCODING_UNKNOWN  0

/** The FilePath character encoding is locale-dependent */
#define GBW_FILEPATH_ENCODING_LOCALE   1

/** The FilePath character encoding is UTF-8 */
#define GBW_FILEPATH_ENCODING_UTF8     2

/**
 * Determine the encoding used internally by the FilePath functions
 * @param style points to a variable which receives the encoding style flag
 * @param p the pool to allocate any working storage
 * @remark Use @c gbw_os_locale_encoding and/or @c gbw_os_default_encoding
 * to get the name of the path encoding if it's not UTF-8.
 */
int gbw_filepath_encoding(int *style, gbw_pool_t *p);
/** @} */

/** @} */

mode_t gbw_perms2mode(int32_t perm);

int32_t gbw_mode2perms(mode_t mode);


#endif /* GBW_FILE_INFO_H */
