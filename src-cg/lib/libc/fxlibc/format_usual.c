#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fxlibc/printf.h>

/* Character formatter (%c)
    (-)   Move spaces to the right
   {len}  Specifies numbers of (whitespace-padded) characters to print */
void __printf_format_c(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	int c = va_arg(*args, int);

	struct __printf_geometry g = {
		.prefix = 0, .sign = 0, .content = 1,
	};
	__printf_compute_geometry(opt, &g);

	__printf_outn(out, ' ', g.left_spaces);

	/* When using %lc, output as UTF-8 */
	if(opt->size != 4 || c <= 0x7f) {
		__printf_out(out, c);
	}
	else if(c <= 0x7ff) {
		__printf_out(out, 0xc0 | (c >> 6));
		__printf_out(out, 0x80 | (c & 0x3f));
	}
	else if(c <= 0xffff) {
		__printf_out(out, 0xe0 | (c >> 12));
		__printf_out(out, 0x80 | ((c >> 6) & 0x3f));
		__printf_out(out, 0x80 | (c & 0x3f));
	}
	else {
		__printf_out(out, 0xf0 | (c >> 18));
		__printf_out(out, 0x80 | ((c >> 12) & 0x3f));
		__printf_out(out, 0x80 | ((c >> 6) & 0x3f));
		__printf_out(out, 0x80 | (c & 0x3f));
	}

	__printf_outn(out, ' ', g.right_spaces);
}

/* String formatter (%s)
    (-)   Move spaces to the right
   {len}  Minimal numbers of characters to output
   {pre}  Maximal numbers of bytes to read from argument */
void __printf_format_s(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	char const *s = va_arg(*args, char const *);

	/* Compute content length, which is the smallest of two sizes: the
	   length set as precision and the actual length of the string */
	size_t len = 0;
	uint32_t precision = opt->precision ? opt->precision : (-1);
	while(s[len] && len < precision) len++;
	/* Cap precision to real value for __printf_compute_geometry() */
	opt->precision = len;

	struct __printf_geometry g = {
		.prefix = 0, .sign = 0, .content = len,
	};
	__printf_compute_geometry(opt, &g);

	__printf_outn(out, ' ', g.left_spaces);
	for(size_t i = 0; i < len; i++) __printf_out(out, s[i]);
	__printf_outn(out, ' ', g.right_spaces);
}

/*Integer formatter (%d, %i)
    (0)   Pad with zeros, rather than spaces, on the left
    (-)   Move spaces to the right (overrides '0', extends {pre})
    ( )   Force a blank sign before nonnegative numbers
    (+)   Force a sign before every number (overrides ' ')
   {len}  Minimal number of characters to print
   {pre}  Forces a minimal number of digits, creating 0s (overrides '0') */
void __printf_format_di(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	int64_t n = __printf_load_i(opt->size, args);

	/* Compute the sign and the absolute value */
	struct __printf_geometry g = {
		.sign   = (n < 0) ? '-' : '+',
		.prefix = 0,
		.style  = _PRINTF_INTEGER,
	};
	if(n < 0) n = -n;

	/* Get the digit string */
	char digits[32];
	int pure, total;

	pure = __printf_digits10(digits, n);
	if(opt->precision == 0 && !n) pure = 0;
	total = (pure > opt->precision ? pure : opt->precision);
	g.content = total;

	__printf_compute_geometry(opt, &g);

	/* Print the result */
	__printf_outn(out, ' ', g.left_spaces);
	if(g.sign) __printf_out(out, g.sign);
	__printf_outn(out, '0', g.zeros);

	__printf_outn(out, '0', total - pure);
	for(int i = pure - 1; i >= 0; i--) __printf_out(out, digits[i]);

	__printf_outn(out, ' ', g.right_spaces);
}

/* Unsigned integer formatter in various bases (%u, %o, %x)
    (#)   Enable base prefixes ("0" in octal, "0x" in hexadecimal)
    (0)   Pad with zeros, rather than spaces, on the left
    (-)   Move spaces to the right (overrides '0', extends {pre})
   {len}  Minimal number of characters to print
   {pre}  Forces a minimal number of digits, creating 0s (overrides '0') */
void __printf_format_ouxX(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	uint64_t n = __printf_load_u(opt->size, args);

	char digits[48];
	int pure = 0, total;
	int specl = opt->spec | 0x20;

	if(specl == 'u') pure = __printf_digits10(digits, n);
	if(specl == 'o') pure = __printf_digits8(digits, n);
	if(specl == 'x') pure = __printf_digits16(digits, opt->spec == 'X', n);

	if(opt->precision == 0 && !n) pure = 0;
	total = (pure > opt->precision ? pure : opt->precision);

	/* Prefix length */
	size_t prefix = 0;
	if(opt->alternative) prefix = (specl != 'u') + (specl == 'x');

	struct __printf_geometry g = {
		.sign = 0, .prefix = prefix, .content = total,
		.style = _PRINTF_INTEGER,
	};
	__printf_compute_geometry(opt, &g);

	/* Output */
	__printf_outn(out, ' ', g.left_spaces);
	if(opt->alternative)
	{
		if(specl != 'u') __printf_out(out, '0');
		if(specl == 'x') __printf_out(out, opt->spec);
	}
	__printf_outn(out, '0', g.zeros);

	__printf_outn(out, '0', total - pure);
	for(int i = pure - 1; i >= 0; i--) __printf_out(out, digits[i]);
	__printf_outn(out, ' ', g.right_spaces);
}

/* Pointer formatter (%p) */
void __printf_format_p(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	(void)opt;
	void *p = va_arg(*args, void *);

	char digits[] = "00000000";
	__printf_digits16(digits, 0, (uint32_t)p);

	__printf_out(out, '0');
	__printf_out(out, 'x');
	for(int i = 7; i >= 0; i--) __printf_out(out, digits[i]);
}

/* errno message formatter (%m) */
void __printf_format_m(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	(void)opt;
	(void)args;

	char const *message = strerror(errno);
	__printf_outstr(out, message, strlen(message));
}

/* Number of characters written so far (%n) */
void __printf_format_n(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	void *p = va_arg(*args, void *);

	if(opt->size == 0) *(char *)p      = out->count;
	if(opt->size == 1) *(short *)p     = out->count;
	if(opt->size == 2) *(int *)p       = out->count;
	if(opt->size == 3) *(long *)p      = out->count;
	if(opt->size == 4) *(long long *)p = out->count;
}
