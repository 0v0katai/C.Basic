#include "casiowin.h"

typedef int32_t off_t;
typedef int32_t ssize_t;

ssize_t write(int fd, void const *buf, size_t count);
ssize_t read(int fd, void *buf, size_t nbytes);
off_t lseek(int fd, off_t offset, int whence);

ssize_t write(int fd, void const *buf, size_t count)
{
    (void)fd;
    (void)buf;
    (void)count;
    return -1;
}

ssize_t read(int fd, void *buf, size_t nbytes)
{
    (void)fd;
    (void)buf;
    (void)nbytes;
    return -1;
}

off_t lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    return -1;
}

void *fxlibc_hal_malloc(size_t size)
{
    return sys_malloc(size);
}

void fxlibc_hal_free(void *ptr)
{
    return sys_free(ptr);
}

void *fxlibc_hal_realloc(void *ptr, size_t size)
{
    return sys_realloc(ptr, size);
}

void _Exit(int status) {
    while (1)
        GetKey(&status);
}
