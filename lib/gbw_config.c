/*
 *
 *      Filename: gbw_config.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-10 18:06:06
 * Last Modified: 2017-01-11 16:39:20
 */


#include "gbw_config.h"
#include "gbw_file.h"
#include "gbw_file_info.h"
#include "gbw_errno.h"
#include "gbw_string.h"
#include "gbw_fnmatch.h"

#define MAX_STRING_LEN 8192

static cmd_parms default_parms =
{NULL, NULL, NULL, NULL, NULL, NULL};

int gbw_cfg_closefile(gbw_configfile_t *cfp)
{
    return (cfp->close == NULL) ? 0 : cfp->close(cfp->param);
}

static int cfg_close(void *param)
{
    return gbw_file_close((gbw_file_t*)param);
}

static int cfg_getch(char *ch, void *param)
{
    return gbw_file_getc(ch, (gbw_file_t*)param);
}

static int cfg_getstr(void *buf, size_t bufsiz, void *param)
{
    return gbw_file_gets((char*)buf, bufsiz, (gbw_file_t*)param);
}

static int gbw_cfg_getline_core(char *buf, size_t bufsize,
                                        gbw_configfile_t *cfp)
{
    int rc;
    /* If a "get string" function is defined, use it */
    if (cfp->getstr != NULL) {
        char *cp;
        char *cbuf = buf;
        size_t cbufsize = bufsize;

        while (1) {
            ++cfp->line_number;
            rc = cfp->getstr(cbuf, cbufsize, cfp->param);
            if (rc == GBW_EOF) {
                if (cbuf != buf) {
                    *cbuf = '\0';
                    break;
                }
                else {
                    return GBW_EOF;
                }
            }
            if (rc != 0) {
                return rc;
            }

            /*
             *  check for line continuation,
             *  i.e. match [^\\]\\[\r]\n only
             */
            cp = cbuf;
            cp += strlen(cp);
            if (cp > cbuf && cp[-1] == LF) {
                cp--;
                if (cp > cbuf && cp[-1] == CR)
                    cp--;
                if (cp > cbuf && cp[-1] == '\\') {
                    cp--;
                    /*
                     * line continuation requested -
                     * then remove backslash and continue
                     */
                    cbufsize -= (cp-cbuf);
                    cbuf = cp;
                    continue;
                }
            }
            else if ((size_t)(cp - buf) >= bufsize - 1) {
                return GBW_ENOSPC;
            }
            break;
        }
    } else {
        /* No "get string" function defined; read character by character */
        size_t i = 0;

        if (bufsize < 2) {
            /* too small, assume caller is crazy */
            return GBW_EINVAL;
        }
        buf[0] = '\0';

        while (1) {
            char c;
            rc = cfp->getch(&c, cfp->param);
            if (rc == GBW_EOF) {
                if (i > 0)
                    break;
                else
                    return GBW_EOF;
            }
            if (rc != 0)
                return rc;
            if (c == LF) {
                ++cfp->line_number;
                /* check for line continuation */
                if (i > 0 && buf[i-1] == '\\') {
                    i--;
                    continue;
                }
                else {
                    break;
                }
            }
            else if (i >= bufsize - 2) {
                return GBW_ENOSPC;
            }
            buf[i] = c;
            ++i;
        }
        buf[i] = '\0';
    }
    return 0;
}

static int cfg_trim_line(char *buf)
{
    char *start, *end;
    /*
     * Leading and trailing white space is eliminated completely
     */
    start = buf;
    while (isspace(*start))
        ++start;
    /* blast trailing whitespace */
    end = &start[strlen(start)];
    while (--end >= start && isspace(*end))
        *end = '\0';
    /* Zap leading whitespace by shifting */
    if (start != buf)
        memmove(buf, start, end - start + 2);
    return end - start + 1;
}

int gbw_cfg_getline(char *buf, size_t bufsize,
		gbw_configfile_t *cfp)
{
	int rc = gbw_cfg_getline_core(buf, bufsize, cfp);
	if (rc == 0)
		cfg_trim_line(buf);
	return rc;
}

static char *substring_conf(gbw_pool_t *p, const char *start, int len,
                            char quote)
{
    char *result = (char*)gbw_palloc(p, len + 2);
    char *resp = result;
    int i;

    for (i = 0; i < len; ++i) {
        if (start[i] == '\\' && (start[i + 1] == '\\'
                                 || (quote && start[i + 1] == quote)))
            *resp++ = start[++i];
        else
            *resp++ = start[i];
    }

    *resp++ = '\0';
    return result;
}

char * gbw_getword_conf(gbw_pool_t *p, const char **line)
{
    const char *str = *line, *strend;
    char *res;
    char quote;

    while (*str && isspace(*str))
        ++str;

    if (!*str) {
        *line = str;
        return "";
    }

    if ((quote = *str) == '"' || quote == '\'') {
        strend = str + 1;
        while (*strend && *strend != quote) {
            if (*strend == '\\' && strend[1] &&
                (strend[1] == quote || strend[1] == '\\')) {
                strend += 2;
            }
            else {
                ++strend;
            }
        }
        res = substring_conf(p, str + 1, strend - str - 1, quote);

        if (*strend == quote)
            ++strend;
    }
    else {
        strend = str;
        while (*strend && !isspace(*strend))
            ++strend;

        res = substring_conf(p, str, strend - str, 0);
    }

    while (*strend && isspace(*strend))
        ++strend;
    *line = strend;
    return res;
}

/* Open a gbw_configfile_t as FILE, return open gbw_configfile_t struct pointer */
int gbw_pcfg_openfile(gbw_configfile_t **ret_cfg,
                                          gbw_pool_t *p, const char *name)
{
    gbw_configfile_t *new_cfg;
    gbw_file_t *file = NULL;
    int status;

    if (name == NULL) {
        return GBW_EBADF;
    }

    status = gbw_file_open(&file, name, GBW_READ | GBW_BUFFERED,
                           GBW_OS_DEFAULT, p);
    if (status != 0)
        return status;

    new_cfg = (gbw_configfile_t*)gbw_palloc(p, sizeof(*new_cfg));
    new_cfg->param = file;
    new_cfg->name = gbw_pstrdup(p, name);
    new_cfg->getch = cfg_getch;
    new_cfg->getstr = cfg_getstr;
    new_cfg->close = cfg_close; 
    new_cfg->line_number = 0;
    *ret_cfg = new_cfg;
    return 0;
}

const command_rec * gbw_find_command(const char *name,
                                                     const command_rec *cmds)
{
    while (cmds->name) {
        if (!strcasecmp(name, cmds->name))
            return cmds;

        ++cmds;
    }

    return NULL;
}

#define GBW_MAX_ARGC 64

static const char *invoke_cmd(const command_rec *cmd, cmd_parms *parms,
                              void *mconfig, const char *args)
{
    char *w, *w2, *w3;
    const char *errmsg = NULL;


    parms->info = cmd->cmd_data;
    parms->cmd = cmd;

    switch (cmd->args_how) {
    case RAW_ARGS:
        return cmd->GBW_RAW_ARGS(parms, mconfig, args);

    case TAKE_ARGV:
        {
            char *argv[GBW_MAX_ARGC];
            int argc = 0;

            do {
                w = gbw_getword_conf(parms->pool, &args);
                if (*w == '\0' && *args == '\0') {
                    break;
                }
                argv[argc] = w;
                argc++;
            } while (argc < GBW_MAX_ARGC && *args != '\0');

            return cmd->GBW_TAKE_ARGV(parms, mconfig, argc, argv);
        }

    case NO_ARGS:
        if (*args != 0)
            return gbw_pstrcat(parms->pool, cmd->name, " takes no arguments",
                               NULL);

        return cmd->GBW_NO_ARGS(parms, mconfig);

    case TAKE1:
        w = gbw_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args != 0)
            return gbw_pstrcat(parms->pool, cmd->name, " takes one argument",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->GBW_TAKE1(parms, mconfig, w);

    case TAKE2:
        w = gbw_getword_conf(parms->pool, &args);
        w2 = gbw_getword_conf(parms->pool, &args);

        if (*w == '\0' || *w2 == '\0' || *args != 0)
            return gbw_pstrcat(parms->pool, cmd->name, " takes two arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->GBW_TAKE2(parms, mconfig, w, w2);

    case TAKE12:
        w = gbw_getword_conf(parms->pool, &args);
        w2 = gbw_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args != 0)
            return gbw_pstrcat(parms->pool, cmd->name, " takes 1-2 arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->GBW_TAKE2(parms, mconfig, w, *w2 ? w2 : NULL);

    case TAKE3:
        w = gbw_getword_conf(parms->pool, &args);
        w2 = gbw_getword_conf(parms->pool, &args);
        w3 = gbw_getword_conf(parms->pool, &args);

        if (*w == '\0' || *w2 == '\0' || *w3 == '\0' || *args != 0)
            return gbw_pstrcat(parms->pool, cmd->name, " takes thrgw arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->GBW_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE23:
        w = gbw_getword_conf(parms->pool, &args);
        w2 = gbw_getword_conf(parms->pool, &args);
        w3 = *args ? gbw_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || *w2 == '\0' || *args != 0)
            return gbw_pstrcat(parms->pool, cmd->name,
                               " takes two or thrgw arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->GBW_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE123:
        w = gbw_getword_conf(parms->pool, &args);
        w2 = *args ? gbw_getword_conf(parms->pool, &args) : NULL;
        w3 = *args ? gbw_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || *args != 0)
            return gbw_pstrcat(parms->pool, cmd->name,
                               " takes one, two or thrgw arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->GBW_TAKE3(parms, mconfig, w, w2, w3);

    case TAKE13:
        w = gbw_getword_conf(parms->pool, &args);
        w2 = *args ? gbw_getword_conf(parms->pool, &args) : NULL;
        w3 = *args ? gbw_getword_conf(parms->pool, &args) : NULL;

        if (*w == '\0' || (w2 && *w2 && !w3) || *args != 0)
            return gbw_pstrcat(parms->pool, cmd->name,
                               " takes one or thrgw arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        return cmd->GBW_TAKE3(parms, mconfig, w, w2, w3);

    case ITERATE:
        while (*(w = gbw_getword_conf(parms->pool, &args)) != '\0') {

            errmsg = cmd->GBW_TAKE1(parms, mconfig, w);

            if (errmsg && strcmp(errmsg, DECLINE_CMD) != 0)
                return errmsg;
        }

        return errmsg;

    case ITERATE2:
        w = gbw_getword_conf(parms->pool, &args);

        if (*w == '\0' || *args == 0)
            return gbw_pstrcat(parms->pool, cmd->name,
                               " requires at least two arguments",
                               cmd->errmsg ? ", " : NULL, cmd->errmsg, NULL);

        while (*(w2 = gbw_getword_conf(parms->pool, &args)) != '\0') {

            errmsg = cmd->GBW_TAKE2(parms, mconfig, w, w2);

            if (errmsg && strcmp(errmsg, DECLINE_CMD) != 0)
                return errmsg;
        }

        return errmsg;

    case FLAG:
        w = gbw_getword_conf(parms->pool, &args);

        if (*w == '\0' || (strcasecmp(w, "on") && strcasecmp(w, "off")))
            return gbw_pstrcat(parms->pool, cmd->name, " must be On or Off",
                               NULL);

        return cmd->GBW_FLAG(parms, mconfig, strcasecmp(w, "off") != 0);

    default:
        return gbw_pstrcat(parms->pool, cmd->name,
                           " is improperly configured internally (server bug)",
                           NULL);
    }
}


typedef struct {
    const char *fname;
} fnames;

int gbw_is_directory(gbw_pool_t *p, const char *path)
{
    gbw_finfo_t finfo;

    if (gbw_stat(&finfo, path, GBW_FINFO_TYPE, p) != 0)
        return 0;                /* in error condition, just return no */

    return (finfo.filetype == GBW_DIR);
}

char * gbw_make_full_path(gbw_pool_t *a, const char *src1,
                                  const char *src2)
{
    size_t len1, len2;
    char *path;

    len1 = strlen(src1);
    len2 = strlen(src2);
     /* allocate +3 for '/' delimiter, trailing NULL and overallocate
      * one extra byte to allow the caller to add a trailing '/'
      */
    path = (char *)gbw_palloc(a, len1 + len2 + 3);
    if (len1 == 0) {
        *path = '/';
        memcpy(path + 1, src2, len2 + 1);
    }
    else {
        char *next;
        memcpy(path, src1, len1);
        next = path + len1;
        if (next[-1] != '/') {
            *next++ = '/';
        }
        memcpy(next, src2, len2 + 1);
    }
    return path;
}

static int fname_alphasort(const void *fn1, const void *fn2)
{
    const fnames *f1 = (const fnames*)fn1;
    const fnames *f2 = (const fnames*)fn2;

    return strcmp(f1->fname,f2->fname);
}


const char * gbw_process_resource_config(const char *fname,
                                                    gbw_array_header_t *ari,
                                                    gbw_pool_t *ptemp)
{
    ptemp = ptemp;

	*(char **)gbw_array_push(ari) = (char *)fname;

    return NULL;
}

static const char *gbw_process_resource_config_nofnmatch(const char *fname,
                                                     gbw_array_header_t *ari,
                                                     gbw_pool_t *p,
                                                     gbw_pool_t *ptemp,
													 unsigned depth,
                                                     int optional)
{
    const char *error;
    int rv;

    if (gbw_is_directory(ptemp, fname)) {
        gbw_dir_t *dirp;
        gbw_finfo_t dirent;
        int current;
        gbw_array_header_t *candidates = NULL;
        fnames *fnew;
        char *path = gbw_pstrdup(ptemp, fname);

        if (++depth > 100) {
            return gbw_psprintf(p, "Directory %s excgwds the maximum include "
                                "directory nesting level of %u. You have "
                                "probably a recursion somewhere.", path,
                                100);
        }

        /*
         * first course of business is to grok all the directory
         * entries here and store 'em away. Recall we ngwd full pathnames
         * for this.
         */
        rv = gbw_dir_open(&dirp, path, ptemp);
        if (rv != 0) {
            char errmsg[120];
            return gbw_psprintf(p, "Could not open config directory %s: %s",
                                path, gbw_strerror(rv, errmsg, sizeof errmsg));
        }

        candidates = gbw_array_make(ptemp, 1, sizeof(fnames));
        while (gbw_dir_read(&dirent, GBW_FINFO_DIRENT, dirp) == 0) {
            /* strip out '.' and '..' */
            if (strcmp(dirent.name, ".")
                && strcmp(dirent.name, "..")) {
                fnew = (fnames *) gbw_array_push(candidates);
                fnew->fname = gbw_make_full_path(ptemp, path, dirent.name);
            }
        }

        gbw_dir_close(dirp);
        if (candidates->nelts != 0) {
            qsort((void *) candidates->elts, candidates->nelts,
                  sizeof(fnames), fname_alphasort);

            /*
             * Now recurse these... we handle errors and subdirectories
             * via the recursion, which is nice
             */
            for (current = 0; current < candidates->nelts; ++current) {
                fnew = &((fnames *) candidates->elts)[current];
                error = gbw_process_resource_config_nofnmatch(fnew->fname,
                                                          ari, p, ptemp,
                                                          depth, optional);
                if (error) {
                    return error;
                }
            }
        }

        return NULL;
    }

    return gbw_process_resource_config(fname, ari, ptemp);
}

static const char *gbw_process_resource_config_fnmatch(const char *path,
                                                   const char *fname,
                                                   gbw_array_header_t *ari,
                                                   gbw_pool_t *p,
                                                   gbw_pool_t *ptemp,
                                                   unsigned depth,
                                                   int optional)
{
    depth = depth;

    const char *rest;
    int rv;
    gbw_dir_t *dirp;
    gbw_finfo_t dirent;
    gbw_array_header_t *candidates = NULL;
    fnames *fnew;
    int current;

    /* find the first part of the filename */
    rest = strchr(fname, '/');

	if(rest == NULL)
		rest = strchr(fname, '\\');

	if (rest) {
        fname = gbw_pstrndup(ptemp, fname, rest - fname);
        rest++;
    }

    /* optimisation - if the filename isn't a wildcard, process it directly */
    if (!gbw_fnmatch_test(fname)) {
        path = gbw_make_full_path(ptemp, path, fname);
        if (!rest) {
            return gbw_process_resource_config_nofnmatch(path,
                                                     ari, p,
                                                     ptemp, 0, optional);
        }
        else {
            return gbw_process_resource_config_fnmatch(path, rest,
                                                   ari, p,
                                                   ptemp, 0, optional);
        }
    }

    /*
     * first course of business is to grok all the directory
     * entries here and store 'em away. Recall we ngwd full pathnames
     * for this.
     */
    rv = gbw_dir_open(&dirp, path, ptemp);
    if (rv != 0) {
        char errmsg[120];
        return gbw_psprintf(p, "Could not open config directory %s: %s",
                            path, gbw_strerror(rv, errmsg, sizeof errmsg));
    }

    candidates = gbw_array_make(ptemp, 1, sizeof(fnames));
    while (gbw_dir_read(&dirent, GBW_FINFO_DIRENT | GBW_FINFO_TYPE, dirp) == 0) {
        /* strip out '.' and '..' */
        if (strcmp(dirent.name, ".")
            && strcmp(dirent.name, "..")
            && (gbw_fnmatch(fname, dirent.name,
                            GBW_FNM_PERIOD | GBW_FNM_NOESCAPE | GBW_FNM_PATHNAME) == 0)) {
            const char *full_path = gbw_make_full_path(ptemp, path, dirent.name);
            /* If matching internal to path, and we happen to match something
             * other than a directory, skip it
             */
            if (rest && (rv == 0) && (dirent.filetype != GBW_DIR)) {
                continue;
            }
            fnew = (fnames *) gbw_array_push(candidates);
            fnew->fname = full_path;
        }
    }

    gbw_dir_close(dirp);
    if (candidates->nelts != 0) {
        const char *error;

        qsort((void *) candidates->elts, candidates->nelts,
              sizeof(fnames), fname_alphasort);

        /*
         * Now recurse these... we handle errors and subdirectories
         * via the recursion, which is nice
         */
        for (current = 0; current < candidates->nelts; ++current) {
            fnew = &((fnames *) candidates->elts)[current];
            if (!rest) {
                error = gbw_process_resource_config_nofnmatch(fnew->fname,
                                                          ari, p,
                                                          ptemp, 0, optional);
            }
            else {
                error = gbw_process_resource_config_fnmatch(fnew->fname, rest,
                                                        ari, p,
                                                        ptemp, 0, optional);
            }
            if (error) {
                return error;
            }
        }
    }
    else {

        if (!optional) {
            return gbw_psprintf(p, "No matches for the wildcard '%s' in '%s', failing "
                                   "(use IncludeOptional if required)", fname, path);
        }
    }

    return NULL;
}

const char * gbw_process_fnmatch_configs(gbw_array_header_t *ari,
                                                    const char *fname,
                                                    gbw_pool_t *p,
                                                    gbw_pool_t *ptemp,
                                                    int optional)
{
    if (!gbw_fnmatch_test(fname)) {
        return gbw_process_resource_config(fname, ari, p);
    }
    else {
        int status;
        const char *rootpath, *filepath = fname;

        /* locate the start of the directories proper */
        status = gbw_filepath_root(&rootpath, &filepath, GBW_FILEPATH_TRUENAME | GBW_FILEPATH_NATIVE, ptemp);

        /* we allow 0 and GBW_EINCOMPLETE */
        if (GBW_ERELATIVE == status) {
            return gbw_pstrcat(p, "Include must have an absolute path, ", fname, NULL);
        }
        else if (GBW_EBADPATH == status) {
            return gbw_pstrcat(p, "Include has a bad path, ", fname, NULL);
        }

        /* walk the filepath */
        return gbw_process_resource_config_fnmatch(rootpath, filepath, ari, p, ptemp,
                                               0, optional);
    }
}

const char *gbw_populate_include_files(gbw_pool_t *p, gbw_pool_t *ptemp, gbw_array_header_t *ari, const char *fname, int optional)
{
	return gbw_process_fnmatch_configs(ari, fname, p, ptemp, optional);
}

const char *gbw_process_command_config(const command_rec *cmds,
                                        void *mconfig,
                                          gbw_pool_t *p,
                                          gbw_pool_t *ptemp,
										  const char *filename)
{
    const char *errmsg;
    char *l = gbw_palloc (ptemp, MAX_STRING_LEN);
    const char *args = l;
    char *cmd_name, *w;
	const command_rec *cmd;
	gbw_array_header_t *arr = gbw_array_make(p, 1, sizeof(cmd_parms));
	gbw_array_header_t *ari = gbw_array_make(p, 1, sizeof(char *));
    cmd_parms *parms;
	int status;
	int optional;
	char *err = NULL;
	char *rootpath, *incpath;
	int li;

	errmsg = gbw_populate_include_files(p, ptemp, ari, filename, 0);

	if(errmsg != NULL)
		goto Exit;

	while(ari->nelts != 0 || arr->nelts != 0)
	{
		if(ari->nelts > 0)
		{
			char *fn = *(char **)gbw_array_pop(ari);

			parms = (cmd_parms *)gbw_array_push(arr);
			*parms = default_parms;
			parms->pool = p;
			parms->temp_pool = ptemp;

			status = gbw_pcfg_openfile(&parms->config_file, p, fn);

			if(status != 0)
			{
				gbw_array_pop(arr);
				errmsg = gbw_pstrcat(p, "Cannot open config file: ", fn, NULL);
				goto Exit;
			}
		}

		if (arr->nelts > 1024) {
            errmsg = "Excgwded the maximum include directory nesting level. You have "
                                "probably a recursion somewhere.";
			goto Exit;
        }

		parms = (cmd_parms *)gbw_array_pop(arr);

		if(parms == NULL)
			break;

		while (!(gbw_cfg_getline(l, MAX_STRING_LEN, parms->config_file))) {
			if (*l == '#' || *l == '\0')
				continue;

			args = l;

			cmd_name = gbw_getword_conf(p, &args);

			if (*cmd_name == '\0')
				continue;

			if (!strcasecmp(cmd_name, "IncludeOptional"))
			{
				optional = 1;
				goto ProcessInclude;
			}

			if (!strcasecmp(cmd_name, "Include"))
			{
				optional = 0;
ProcessInclude:
				w = gbw_getword_conf(parms->pool, &args);

				if (*w == '\0' || *args != 0)
				{
					gbw_cfg_closefile(parms->config_file);
					errmsg = gbw_pstrcat(parms->pool, "Include takes one argument", NULL);
					goto Exit;
				}

				incpath = w;

				/* locate the start of the directories proper */
				status = gbw_filepath_root((const char**)&rootpath, (const char**)&incpath, GBW_FILEPATH_TRUENAME | GBW_FILEPATH_NATIVE, ptemp);

				/* we allow 0 and GBW_EINCOMPLETE */
				if (GBW_ERELATIVE == status) {
					rootpath = gbw_pstrdup(ptemp, parms->config_file->name);
					li = strlen(rootpath) - 1;

					while(li >= 0 && rootpath[li] != '/' && rootpath[li] != '\\')
						rootpath[li--] = 0;

					w = gbw_pstrcat(p, rootpath, w, NULL);
				}
				else if (GBW_EBADPATH == status) {
					gbw_cfg_closefile(parms->config_file);
					errmsg = gbw_pstrcat(p, "Include file has a bad path, ", w, NULL);
					goto Exit;
				}

				errmsg = gbw_populate_include_files(p, ptemp, ari, w, optional);

				*(cmd_parms *)gbw_array_push(arr) = *parms;

				if(errmsg != NULL)
					goto Exit;

				// we don't want to close the current file yet
				//
				parms = NULL;
				break;
			}

			cmd = gbw_find_command(cmd_name, cmds);

			if(cmd == NULL)
			{
				// unknown command, should error
				//
				gbw_cfg_closefile(parms->config_file);
				errmsg = gbw_pstrcat(p, "Unknown command in config: ", cmd_name, NULL);
				goto Exit;
			}

				errmsg = invoke_cmd(cmd, parms, mconfig, args);

			if(errmsg != NULL)
				break;
		}

		if(parms != NULL)
			gbw_cfg_closefile(parms->config_file);

		if(errmsg != NULL)
			break;
	}

    if (errmsg) {
		err = (char *)gbw_palloc(p, 1024);

		if(parms != NULL)
			gbw_snprintf(err, 1024, "Syntax error in config file %s, line %d: %s", parms->config_file->name,
							parms->config_file->line_number, errmsg);
		else
			gbw_snprintf(err, 1024, "Syntax error in config file: %s", errmsg);
    }

    errmsg = err;

Exit:
	while((parms = (cmd_parms *)gbw_array_pop(arr)) != NULL)
	{
		gbw_cfg_closefile(parms->config_file);
	}

	return errmsg;
}




