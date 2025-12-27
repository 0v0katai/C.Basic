#ifndef __FXLIBC_PRINTF_H__
# define __FXLIBC_PRINTF_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** This headers covers fxlibc-specific extensions to the *printf API. fxlibc
** provides an interface to define new converters, and a number of helpers to
** parse options, lay out strings, and generate output.
*/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

/* Predefined formatters. */

/*
** Enable floating-point formatters %e, %E, %f, %F, %g, and %G. The formats are
** disabled by default because the representation algorithm has tables of
** powers of 10 and quite a bit of code, resulting in 10-15 kiB of additional
** size in every binary.
**
** Calling this functions pulls object files with floating-point representation
** code from the fxlibc library and registers formatters for all 6
** floating-point formats.
*/
extern void __printf_enable_fp(void);

/*
** Enable the fixed-point formatter %D. This format is a nonstandard extension
** for decimal fixed-point. It works like %d, except that the precision field
** represents a number of decimals.
**
** For instance, "%.3D" will print value 12345 as "12.345". This is simply
** integer display with a decimal dot inserted, and useful for applications
** that want some degree of decimal freedom without full-blown floating-point.
*/
extern void __printf_enable_fixed(void);



/* Base function for all formatted printing functions. */

/*
** Output specification; only one of the targets may be non-trivial.
** -> str != NULL
** -> fp != NULL
** -> fd >= 0
** The size field can be set independently.
*/
struct __printf_output {
	/* Final output, after buffering */
	char * __restrict__ str;
	FILE *fp;
	int fd;
	/* Size of the final output */
	size_t size;

	/* The following members are set by __printf; do not write to them. */

	/* Current output buffer, output position, and buffer limit */
	char *buffer, *ptr, *limit;
	/* Number of characters written so far */
	size_t count;
};

/* Generic formatted printing. */
extern int __printf(
	struct __printf_output * __restrict__ __out,
	char const * __restrict__ __format,
	va_list *__args);


/* Format extension API. */

struct __printf_format {
	/* Minimal length of formatted string (padding can be added). */
	uint16_t length;
	/* How much significant characters of data, meaning varies. */
	int16_t precision;

	/* Size of targeted integer type (%o, %x, %i, %d, %u), in bytes */
	uint8_t size;
	/* (#) Alternative form: base prefixes, decimal point. */
	uint8_t alternative :1;
	/* ( ) Add a blank sign before nonnegative numbers. */
	uint8_t blank_sign :1;
	/* (+) Always add a sign before a number (overrides ' '). */
	uint8_t force_sign :1;

	/*
	** Alignment options, from lowest priority to highest priority:
	** NUL  By default, align right
	** (0)  Left-pad numerical values with zeros
	** (-)  Align left by adding space on the right, dropping zeros
	*/
	uint8_t alignment;

	/* Format specifier. */
	char spec;
};

/*
** Type of format functions.
** -> __out specifies the output and is used when generating text
** -> __fmt contains the format options and specifier letter
** -> __args is a pointer to the variable list of arguments to read from
*/
typedef void __printf_formatter_t(
	struct __printf_output *__out,
	struct __printf_format *__fmt,
	va_list *__args);

/*
** Register a new format.
**
** The formatter designated by the specified lowercase or uppercase letter
** (eg 'p' or 'P') is registered. This functions allows overriding default
** formatters, but this is very much discouraged. Letters with special meaning
** in the standard cannot be changed. A formatter can be removed of disabled by
** registering NULL.
**
** Here are the characters used/reserved in the C standard:
**
** a: Hexadecimal floating-point      A: Hexadecimal floating-point
** b: _                               B: _
** c: Character                       C: Deprecated synonym for lc
** d: Decimal integer                 D: _
** e: Exponent floating-point         E: Exponent floating-point
** f: Floating-point                  F: Floating-point
** g: General floating-point          G: General floating-point
** h: short or char size              H: _
** i: Integer                         I: Locale-aware digits
** j: intmax_t size                   J: _
** k: _                               K: _
** l: long or long long size          L: long double size
** m: Error message from errno        M: _
** n: Number of characters written    N: _
** o: Octal integer                   O: _
** p: Pointer                         P: _
** q: Nonstandard synonym for ll      Q: _
** r: _                               R: _
** s: String                          S: Deprecated synonym for ls
** t: ptrdiff_t size                  T: _
** u: Unsigned decimal integer        U: _
** v: _                               V: _
** w: _                               W: _
** x: Hexadecimal integer             X: Hexadecimal integer
** y: _                               Y: _
** z: size_t size                     Z: Old synonym for z
*/
void __printf_register(int __spec, __printf_formatter_t *__format);



/* Functions for formatters to output characters. */

/* Flush the buffer. (Don't call this directly.) */
extern void __printf_flush(struct __printf_output *__out);

/* Output a single character in the buffer (and possibly flush it). */
static inline void __printf_out(struct __printf_output *__out,
	int __c)
{
	if(__out->ptr >= __out->limit) __printf_flush(__out);
	*(__out->ptr++) = __c;
}

/* Output the same character __n times. */
static inline void __printf_outn(struct __printf_output *__out,
	int __c, int __n)
{
	while(__n-- > 0) __printf_out(__out, __c);
}

/* Output a string. */
static inline void __printf_outstr(struct __printf_output *__out,
	char const *__str, int __n)
{
	for(int i = 0; i < __n; i++) __printf_out(__out, __str[i]);
}



/* Helper functions for formatters. */

/*
** Format geometry helper. The structure of a format is as follows:
**
**                  sign v     |< zeros >| |< content >|
**     _ _ _ _ _ _ _ _ _ + 0 x 0 0 0 0 0 0 8 a c 7 . 3 c _ _ _ _ _ _ _ _ _ _
**     |< left_spaces >|   ^^^ prefix                    |< right_spaces  >|
**
**  The sign character is absent if sign=0, the prefix is specified by length
**  and is also absent if prefix=0.
*/
struct __printf_geometry
{
	uint16_t left_spaces;	/* Spaces before content */
	uint8_t  sign;		/* Sign character (NUL, ' ', '+' or '-') */
	uint8_t  prefix;	/* Base prefix ('0', '0x', etc) length */
	uint16_t zeros;		/* For integer displays, number of zeros */
	uint16_t content;	/* Content length in bytes */
	uint16_t right_spaces;	/* Spaces after content */

	/* Style of display:
	     _PRINTF_GENERIC    Sign ignored, 0-padding ignored
	     _PRINTF_INTEGER    .precision causes 0-padding
	     _PRINTF_NUMERIC    No effect */
	enum { _PRINTF_GENERIC = 0, _PRINTF_INTEGER, _PRINTF_NUMERIC } style;
};

/*
** Calculate the geometry of a format.
**
** The caller provides as input __opt (as it received in the formatter
** function), and the following attributes of __geometry:
**
** - prefix: the length of the desired prefix (if unused, 0)
** - content: the natural content length for the provided data
** - sign: the sign of the input ('+' or '-'); for _PRINTF_GENERIC, 0
** - style, which affects the meaning of options
**
** This function outputs:
** - sign: will be changed to ' ' or NUL (0) depending on options
** - All fields of __geometry that are not part of the input
**
** The algorithm for laying out the format is as follows.
** 1. For numerical and integer formats, turn a "+" sign into " " if
**    __opt->blank_sign is set, "+" if __opt->force_sign is set, NUL otherwise.
** 2. Compute the total amount of padding needed to reach __opt->length.
** 3. In integer style, if a precision is specified and more than content
**    length plus sign and prefix, turn some padding into zeros.
** 4. If numerical and integer styles, if __opt->alignment == '0' turn all the
**    padding into zeros.
** 5. Turn remaining padding into spaces at the left (if opt->alignment == NUL)
**    or right (if opt->alignment == '-').
*/
extern void __printf_compute_geometry(
	struct __printf_format *__opt,
	struct __printf_geometry *__geometry);

/* Load a signed integer (__size is specified in __opt->size). */
extern int64_t __printf_load_i(int size, va_list *args);

/* Load an unsigned integer. */
extern uint64_t __printf_load_u(int size, va_list *args);

/*
** Generate the digits of __n in base 10, starting from the least significant
** digit. Returns number of digits. No NUL terminator is added.
*/
extern int __printf_digits10(char *__str, uint64_t __n);

/* Same in base 16. */
extern int __printf_digits16(char *__str, int uppercase, uint64_t __n);

/* Same in base 8. */
extern int __printf_digits8(char *__str, uint64_t __n);

#ifdef __cplusplus
}
#endif

#endif /*__FXLIBC_PRINTF_H__*/