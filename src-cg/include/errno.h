#ifndef __ERRNO_H__
# define __ERRNO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Initialized to 0 at startup. Currently not TLS, maybe in the future if
   threads are supported. */
extern int errno;

#define EDOM          1   /* Outside of domain of math function. */
#define EILSEQ        2   /* Illegal multi-byte character sequence. */
#define ERANGE        3   /* Range error during text-to-numeric conversion. */
#define EACCES        4   /* File access denied. */
#define EEXIST        5   /* File already exists. */
#define EINVAL        6   /* Invalid argument in general; lots of uses. */
#define ENFILE        7   /* Out of file descriptors. */
#define ENOENT        8   /* File or folder does not exist. */
#define ENOMEM        9   /* System ran out of memory (RAM). */
#define EDQUOT       10   /* Out of inodes or memory space. */
#define ENOSPC       11   /* No space left on device. */
#define ENOTSUP      12   /* Operation not supported. */
#define EBADF        13   /* Invalid file descriptor. */
#define ESPIPE       14   /* File descriptor is unable to seek. */
#define EISDIR       15   /* File descriptor is a directory. */
#define ENOTDIR      16   /* File descriptor is not a directory. */
#define ENOTEMPTY    17   /* Directory is not empty. */
#define EINTR        18   /* Interrupted system call. */
#define EAGAIN       19   /* Resource temporarily unavailable. */
#define EIO          20   /* Input/Output error */
#define ENODEV       21   /* No such device */
#define ENOMEDIUM    22   /* No medium found */
#define EMEDIUMTYPE  23   /* Wrong medium type */

/* Error codes used by libstdc++. */
#define EAFNOSUPPORT 24
#define EADDRINUSE   25
#define EADDRNOTAVAIL 26
#define EISCONN      27
#define E2BIG        28
#define EFAULT       29
#define EPIPE        30
#define ECONNABORTED 31
#define EALREADY     32
#define ECONNREFUSED 33
#define ECONNRESET   34
#define EXDEV        35
#define EDESTADDRREQ 36
#define EBUSY        37
#define ENOEXEC      38
#define EFBIG        39
#define ENAMETOOLONG 40
#define ENOSYS       41
#define EHOSTUNREACH 42
#define ENOTTY       43
#define EMSGSIZE     44
#define ENETDOWN     45
#define ENETRESET    46
#define ENETUNREACH  47
#define ENOBUFS      48
#define ECHILD       49
#define ENOLCK       50
#define ENOMSG       51
#define ENOPROTOOPT  52
#define ENXIO        53
#define ESRCH        54
#define ENOTSOCK     55
#define ENOTCONN     56
#define EINPROGRESS  57
#define EPERM        58
#define EOPNOTSUPP   59
#define EWOULDBLOCK  60
#define EPROTONOSUPPORT 61
#define EROFS        62
#define EDEADLK      63
#define ETIMEDOUT    64
#define EMFILE       65
#define EMLINK       66
#define ELOOP        67
#define EPROTOTYPE   68
#define EBADMSG      69
#define EIDRM        70
#define ENOLINK      71
#define ENODATA      72
#define ENOSR        73
#define ECANCELED    74
#define EOWNERDEAD   75
#define EPROTO       76
#define ENOTRECOVERABLE 77
#define ETIME        78
#define ETXTBUSY     79
#define EOVERFLOW    80

#define ENOTIMPL 0xBEEF

#ifdef __cplusplus
}
#endif

#endif /*__ERRNO_H__*/
