#include <stdio.h>
#include <fxlibc/printf.h>

int sprintf(char * restrict str, char const * restrict fmt, ...)
{
	/* This is valid even if str=NULL. */
	struct __printf_output out = {
		.str = str,
		.size = 65536,
	};

	va_list args;
	va_start(args, fmt);

	int count = __printf(&out, fmt, &args);

	va_end(args);
	return count;
}