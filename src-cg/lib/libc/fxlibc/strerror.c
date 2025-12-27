#include <string.h>
#include <errno.h>

static char *errno_strings [] = {
	[0]           = "Success",
	[EDOM]        = "Numerical argument out of domain",
	[EILSEQ]      = "Invalid or incomplete multibyte or wide character",
	[ERANGE]      = "Numerical result out of range",
	[EACCES]      = "Permission denied",
	[EEXIST]      = "File exists",
	[EINVAL]      = "Invalid argument",
	[ENFILE]      = "Too many open files in system",
	[ENOENT]      = "No such file or directory",
	[ENOMEM]      = "Cannot allocate memory",
	[EDQUOT]      = "Disk quota exceeded",
	[ENOSPC]      = "No space left on device",
	[ENOTSUP]     = "Operation not supported",
	[EBADF]       = "Bad file descriptor",
	[ESPIPE]      = "Illegal seek",
	[EISDIR]      = "Is a directory",
	[ENOTDIR]     = "Not a directory",
	[ENOTEMPTY]   = "Directory not empty",
	[EINTR]       = "Interrupted system call",
	[EAGAIN]      = "Resource temporarily unavailable",
	[EIO]         = "Input/output error",
	[ENODEV]      = "No such device",
	[ENOMEDIUM]   = "No medium found",
	[EMEDIUMTYPE] = "Wrong medium type",
};

char *strerror(int e)
{
	int count = sizeof errno_strings / sizeof errno_strings[0];
	return (e >= 0 && e < count) ? errno_strings[e] : "<Unknown errno>";
}
