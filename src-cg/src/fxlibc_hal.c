#include "casiowin.h"
#include <unistd.h>

size_t write(int fd, void const *buf, size_t count)
{
    (void)fd;
    (void)buf;
    (void)count;
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

static unsigned long next = 1;

int sys_rand(void) {
    next = next * 1103515245 + 12345;
    return((unsigned)(next/65536) % 32768);
}

void sys_srand(unsigned seed) {
    next = seed;
}

void _Exit(int status) {
    while (1)
        GetKey(&status);
}
