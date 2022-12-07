/*
 *
 *      Filename: gbw_errno.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-11 10:35:16
 * Last Modified: 2017-01-11 10:46:51
 */

#include "gbw_errno.h"
#include "gbw_string.h"
/*
 * stuffbuffer - like gbw_cpystrn() but returns the address of the
 * dest buffer instead of the address of the terminating '\0'
 */
static char *stuffbuffer(char *buf, size_t bufsize, const char *s)
{
    gbw_cpystrn(buf,s,bufsize);
    return buf;
}

static char *gbw_error_string(int statcode)
{
    switch (statcode) {
    case GBW_ENOSTAT:
        return "Could not perform a stat on the file.";
    case GBW_ENOPOOL:
        return "A new pool could not be created.";
    case GBW_EBADDATE:
        return "An invalid date has been provided";
    case GBW_EINVALSOCK:
        return "An invalid socket was returned";
    case GBW_ENOPROC:
        return "No process was provided and one was required.";
    case GBW_ENOTIME:
        return "No time was provided and one was required.";
    case GBW_ENODIR:
        return "No directory was provided and one was required.";
    case GBW_ENOLOCK:
        return "No lock was provided and one was required.";
    case GBW_ENOPOLL:
        return "No poll structure was provided and one was required.";
    case GBW_ENOSOCKET:
        return "No socket was provided and one was required.";
    case GBW_ENOTHREAD:
        return "No thread was provided and one was required.";
    case GBW_ENOTHDKEY:
        return "No thread key structure was provided and one was required.";
    case GBW_ENOSHMAVAIL:
        return "No shared memory is currently available";
    case GBW_EDSOOPEN:
        return "DSO load failed";
    case GBW_EBADIP:
        return "The specified IP address is invalid.";
    case GBW_EBADMASK:
        return "The specified network mask is invalid.";
    case GBW_ESYMNOTFOUND:
        return "Could not find the requested symbol.";
    case GBW_ENOTENOUGHENTROPY:
        return "Not enough entropy to continue.";
    case GBW_INCHILD:
        return
	    "Your code just forked, and you are currently executing in the "
	    "child process";
    case GBW_INPARENT:
        return
	    "Your code just forked, and you are currently executing in the "
	    "parent process";
    case GBW_DETACH:
        return "The specified thread is detached";
    case GBW_NOTDETACH:
        return "The specified thread is not detached";
    case GBW_CHILD_DONE:
        return "The specified child process is done executing";
    case GBW_CHILD_NOTDONE:
        return "The specified child process is not done executing";
    case GBW_TIMEUP:
        return "The timeout specified has expired";
    case GBW_INCOMPLETE:
        return "Partial results are valid but processing is incomplete";
    case GBW_BADCH:
        return "Bad character specified on command line";
    case GBW_BADARG:
        return "Missing parameter for the specified command line option";
    case GBW_EOF:
        return "End of file found";
    case GBW_NOTFOUND:
        return "Could not find specified socket in poll list.";
    case GBW_ANONYMOUS:
        return "Shared memory is implemented anonymously";
    case GBW_FILEBASED:
        return "Shared memory is implemented using files";
    case GBW_KEYBASED:
        return "Shared memory is implemented using a key system";
    case GBW_EINIT:
        return
	    "There is no error, this value signifies an initialized "
	    "error code";
    case GBW_ENOTIMPL:
        return "This function has not been implemented on this platform";
    case GBW_EMISMATCH:
        return "passwords do not match";
    case GBW_EABSOLUTE:
        return "The given path is absolute";
    case GBW_ERELATIVE:
        return "The given path is relative";
    case GBW_EINCOMPLETE:
        return "The given path is incomplete";
    case GBW_EABOVEROOT:
        return "The given path was above the root path";
    case GBW_EBADPATH:
        return "The given path is misformatted or contained invalid characters";
    case GBW_EPATHWILD:
        return "The given path contained wildcard characters";
    case GBW_EBUSY:
        return "The given lock was busy.";
    case GBW_EPROC_UNKNOWN:
        return "The process is not recognized.";
    case GBW_EGENERAL:
        return "Internal error (specific information not available)";
    default:
        return "Error string not specified yet";
    }
}



/* On Unix, gbw_os_strerror() handles error codes from the resolver 
 * (h_errno). 
 */
static char *gbw_os_strerror(char* buf, size_t bufsize, int err) 
{
    const char *msg = "";
	err = err;

    return stuffbuffer(buf, bufsize, msg);
}


/* plain old strerror(); 
 * thread-safe on some platforms (e.g., Solaris, OS/390)
 */
static char *native_strerror(int statcode, char *buf,
                             size_t bufsize)
{
    const char *err = strerror(statcode);
    if (err) {
        return stuffbuffer(buf, bufsize, err);
    } else {
        return stuffbuffer(buf, bufsize, 
                           "GW does not understand this error code");
    }
}

char * gbw_strerror(int statcode, char *buf,
                                 size_t bufsize)
{
    if (statcode < GBW_OS_START_ERROR) {
        return native_strerror(statcode, buf, bufsize);
    }
    else if (statcode < GBW_OS_START_USERERR) {
        return stuffbuffer(buf, bufsize, gbw_error_string(statcode));
    }
    else if (statcode < GBW_OS_START_EAIERR) {
        return stuffbuffer(buf, bufsize, "GW does not understand this error code");
    }
    else if (statcode < GBW_OS_START_SYSERR) {
#if defined(HAVE_GAI_STRERROR)
        statcode -= GBW_OS_START_EAIERR;
#if defined(NEGATIVE_EAI)
        statcode = -statcode;
#endif
        return stuffbuffer(buf, bufsize, gai_strerror(statcode));
#else
        return stuffbuffer(buf, bufsize, "GW does not understand this error code");
#endif
    }
    else {
        return gbw_os_strerror(buf, bufsize, statcode - GBW_OS_START_SYSERR);
    }
}


