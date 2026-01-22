#include <stdlib.h>
#include <stddef.h>

extern void *calloc(size_t, size_t);

void *
_calloc_r (struct _reent *ptr, size_t size, size_t len)
{
  return calloc (size, len);
}
