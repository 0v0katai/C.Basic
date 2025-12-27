#include <fxlibc/printf.h>

/* Fixed-point decimal formatter (extension: %D)
    (0)   Pad with zeros, rather than spaces, on the left
    (-)   Move spaces to the right (overrides '0')
    ( )   Force a blank sign before nonnegative numbers
    (+)   Force a sign before every number (overrides ' ')
   {len}  Minimal number of characters to print
   {pre}  Number of digits after the decimal dot */
void __printf_format_D(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	int64_t n = __printf_load_i(opt->size, args);

	/* Compute the sign and the absolute value */
	struct __printf_geometry g = {
		.sign   = (n < 0) ? '-' : '+',
		.prefix = 0,
		.style  = _PRINTF_NUMERIC,
	};
	if(n < 0) n = -n;

	/* Get the digit string */
	char digits[32];

	g.content = __printf_digits10(digits, n) + 1;
	__printf_compute_geometry(opt, &g);

	/* Print the result */
	__printf_outn(out, ' ', g.left_spaces);
	if(g.sign) __printf_out(out, g.sign);
	__printf_outn(out, '0', g.zeros);

	for(int i = g.content - 2; i >= 0; i--)
	{
		if(i == opt->precision - 1) __printf_out(out, '.');
		__printf_out(out, digits[i]);
	}

	__printf_outn(out, ' ', g.right_spaces);
}

void __printf_enable_fixed(void)
{
	__printf_register('D', __printf_format_D);
}
