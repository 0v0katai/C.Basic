#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fxlibc/printf.h>

/* Internal buffer, used when no buffer is specified for output */
#define _PRINTF_BUFSIZE 64
static char __printf_buffer[_PRINTF_BUFSIZE];

/* Notation for a letter that cannot be used as a specifier */
#define _PRINTF_USED ((__printf_formatter_t *)1)

typedef __PTRDIFF_TYPE__ ptrdiff_t;

/* Default formatter functions */
__printf_formatter_t __printf_format_c;
__printf_formatter_t __printf_format_di;
__printf_formatter_t __printf_format_m;
__printf_formatter_t __printf_format_n;
__printf_formatter_t __printf_format_ouxX;
__printf_formatter_t __printf_format_p;
__printf_formatter_t __printf_format_s;

static __printf_formatter_t *__printf_formatters[52] = {
	/* Uppercase letters */
	NULL,			/* A: Hexadecimal floating-point */
	NULL,
	_PRINTF_USED,		/* C: Deprecated synonym for lc */
	NULL,
	NULL,			/* E: Exponent floating-point */
	NULL,			/* F: Floating-point */
	NULL,			/* G: General floating-point */
	NULL,
	_PRINTF_USED,		/* I: Locale-aware digits */
	NULL,
	NULL,
	_PRINTF_USED,		/* L: long double size */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	_PRINTF_USED,		/* S: Deprecated synonym for ls */
	NULL,
	NULL,
	NULL,
	NULL,
	__printf_format_ouxX,	/* X: Hexadecimal integer */
	NULL,
	_PRINTF_USED,		/* Z: Old synonym for z */
	/* Lowercase letters */
	NULL,			/* a: Hexadecimal floating-point */
	NULL,
	__printf_format_c,	/* c: Character */
	__printf_format_di,	/* d: Decimal integer */
	NULL,			/* e: Exponent floating-point */
	NULL,			/* f: Floating-point */
	NULL,			/* g: General floating-point */
	_PRINTF_USED,		/* h: short or char size */
	__printf_format_di,	/* i: Integer */
	_PRINTF_USED,		/* j: intmax_t size */
	NULL,			/* k: _ */
	_PRINTF_USED,		/* l: long or long long size */
	__printf_format_m,	/* m: Error message from errno */
	__printf_format_n,	/* n: Number of characters written */
	__printf_format_ouxX,	/* o: Octal integer */
	__printf_format_p,	/* p: Pointer */
	_PRINTF_USED,		/* q: Nonstandard synonym for ll */
	NULL,			/* r: _ */
	__printf_format_s,	/* s: String */
	_PRINTF_USED,		/* t: ptrdiff_t size */
	__printf_format_ouxX,	/* u: Unsigned decimal integer */
	NULL,			/* v: _ */
	NULL,			/* w: _ */
	__printf_format_ouxX,	/* x: Hexadecimal integer */
	NULL,			/* y: _ */
	_PRINTF_USED,		/* z: size_t size */
};

void __printf_register(int spec, __printf_formatter_t *format)
{
	if(isupper(spec))
		spec = spec - 'A';
	else if(islower(spec))
		spec = spec - 'a' + 26;
	else return;

	if(__printf_formatters[spec] == _PRINTF_USED || format == _PRINTF_USED)
		return;

	__printf_formatters[spec] = format;
}

void __printf_flush(struct __printf_output *out)
{
	/* Update the number of flushed characters */
	out->count += out->ptr - out->buffer;

	/* String: write a NUL at the end even if overriding a character */
	if(out->str && out->str != __printf_buffer)
	{
		char *nul = out->ptr;
		if(out->limit - 1 < nul) nul = out->limit - 1;
		*nul = 0x00;
	}
	/* File pointer: output with fwrite */
	else if(out->fp)
	{
		// fwrite(out->buffer, 1, out->ptr - out->buffer, out->fp);
	}
	/* File pointer: output with write */
	else if(out->fd)
	{
		// write(out->fd, out->buffer, out->ptr - out->buffer);
	}

	/* Switch to the internal buffer (when writing to a string that string
	   is now full, and when writing to files we've always been there) */
	out->buffer = __printf_buffer;
	out->limit  = __printf_buffer + _PRINTF_BUFSIZE;
	out->ptr    = out->buffer;
}

/* Parse format strings. */
static struct __printf_format parse_fmt(char const * restrict *options_ptr)
{
	/* No options enabled by default; default size is set at the end */
	struct __printf_format opt = { .size = 0, .precision = -1 };

	/* This function acts as a deterministic finite automaton */
	enum {
		basic,      /* Reading option characters */
		length,     /* Reading length digits */
		precision,  /* Reading precision digits */
	} state = basic;

	char const *options = *options_ptr;

	/* Previous size letter (used to keep track of hh and ll) */
	int size_letter = 0;

	for(int c; (c = *options); options++)
	{
		int index = -1;
		if(isupper(c)) index = c - 'A';
		if(islower(c)) index = c - 'a' + 26;
		if(index >= 0 && __printf_formatters[index] != _PRINTF_USED)
			break;

		if(c == '.')
		{
			state = precision;
			opt.precision = 0;
			continue;
		}
		else if(state == length && c >= '0' && c <= '9')
		{
			opt.length = opt.length * 10 + (c - '0');
			continue;
		}
		else if(state == precision && c >= '0' && c <= '9')
		{
			opt.precision = opt.precision * 10 + (c - '0');
			continue;
		}

		/* Remain in basic state mode */
		state = basic;

		if(c == '#') opt.alternative = 1;
		if(c == ' ') opt.blank_sign = 1;
		if(c == '+') opt.force_sign = 1;

		/* Alignment options, including priority */
		if((c == '-' || c == '0') && opt.alignment != '0')
		{
			opt.alignment = c;
		}

		/* Data size */
		if(strchr("hlzjtL", c))
		{
			if(c == 'h' && size_letter == 'h')
				opt.size = sizeof(char);
			else if(c == 'h')
				opt.size = sizeof(short);
			else if(c == 'l' && size_letter == 'l')
				opt.size = sizeof(long long);
			else if(c == 'l')
				opt.size = sizeof(long);
			else if(c == 'z')
				opt.size = sizeof(size_t);
			else if(c == 'j')
				opt.size = sizeof(intmax_t);
			else if(c == 't')
				opt.size = sizeof(ptrdiff_t);
			else if(c == 'L')
				opt.size = sizeof(long double);

			size_letter = c;
		}

		if(c >= '1' && c <= '9') state = length, options--;
	}

	*options_ptr = options;
	return opt;
}

/* The formatted printer. */
int __printf(
	struct __printf_output * restrict out,
	char const * restrict format,
	va_list *args)
{
	/* Set up the buffer for the output */
	if(out->fp || out->fd)
	{
		out->buffer = __printf_buffer;
		out->limit = __printf_buffer + _PRINTF_BUFSIZE;
	}
	else
	{
		out->buffer = out->str;
		out->limit = out->str + out->size;
	}
	out->ptr = out->buffer;
	out->count = 0;

	int c;
	struct __printf_format opt;

	while((c = *format++))
	{
		if(c != '%')
		{
			__printf_out(out, c);
			continue;
		}

		if(!(c = *format)) break;

		/* '%' character */
		if(c == '%')
		{
			__printf_out(out, '%');
			format++;
			continue;
		}

		opt = parse_fmt(&format);
		opt.spec = *format++;

		/* Set default size to 1 for %c, 4 otherwise */
		if(opt.size == 0)
			opt.size = (opt.spec == 'c') ? 1 : 4;

		int id;
		if(isupper(opt.spec))
			id = opt.spec - 'A';
		else if(islower(opt.spec))
			id = opt.spec - 'a' + 26;
		else continue;

		__printf_formatter_t *f = __printf_formatters[id];
		if(f == NULL || f == _PRINTF_USED) continue;

		f(out, &opt, args);
	}

	__printf_flush(out);
	return (int)out->count;
}
