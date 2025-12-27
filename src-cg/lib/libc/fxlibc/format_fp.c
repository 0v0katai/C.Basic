#include <stddef.h>
#include <fxlibc/printf.h>
#include "grisu2b_59_56/grisu2.h"

#define min(x, y) ({ \
	__auto_type _x = (x); \
	__auto_type _y = (y); \
	(_x < _y) ? (_x) : (_y); \
})

//---
// String generation for doubles
//---

/* The grisu2() function does not have a size limit so we generate into a
   buffer large enough to hold the result. */
static char digit_buffer[50];

/* Round a number at the specified place (which can be out of bounds). An extra
   byte before the buffer should be secured to leave room for a new digit in
   case a carry reaches there. Returns the new start-of-buffer.

   @buffer   Buffer with generated digits, updated if needed
   @length   Number of digits generated in the buffer, updated if needed
   @e        Location of decimal dot relative to integer in buffer
   @place    Decimal to place to round to */
static void round_str(char **buffer_ptr, int *length, int e, int place)
{
	char *buffer = *buffer_ptr;

	/* Interpret place as relative to buffer indices */
	place += *length + e - 1;

	/* Specified place is out-of-bounds */
	if(place < 0 || place >= *length - 1) return;
	/* Next digit is 0..4 so rounding has no effect */
	if(buffer[place + 1] < '5') return;

	/* Propagate carries if needed */
	while(place >= -1)
	{
		buffer[place]++;
		if(buffer[place] <= '9') break;

		buffer[place] = '0';
		place--;
	}

	/* Add one digit if needed */
	if(place >= 0) return;
	(*buffer_ptr)--;
	(*length)++;
}

/* Remove zeros at the end of the digits, reducing [length] accordingly. */
static int remove_zeros(char *buffer, int length, int unremovable)
{
	int removed = 0;

	while(length > unremovable && buffer[length - 1] == '0')
	{
		buffer[length - 1] = 0;
		length--;
		removed++;
	}

	return removed;
}

/* Handles infinities and NaNs. */
static int special_notation(struct __printf_output *out, double v, int upper)
{
	if(__builtin_isinf(v) && v < 0)
	{
		__printf_outstr(out, upper ? "-INF" : "-inf", 4);
		return 1;
	}
	if(__builtin_isinf(v))
	{
		__printf_outstr(out, upper ? "INF" : "inf", 3);
		return 1;
	}
	if(__builtin_isnan(v))
	{
		__printf_outstr(out, upper ? "NAN" : "nan", 3);
		return 1;
	}
	return 0;
}

/* Prints decimal explicitly for %f and %g. */
static void direct_notation(
	struct __printf_output *out,
	struct __printf_format *opt,
	struct __printf_geometry g,
	char *digits, int length, int e)
{
	/* Number of characters for decimal part, including dot */
	int dec_chars = opt->precision + (opt->precision > 0);

	/* See case discussion below */
	g.content = (length + e >= 0) ? length + e + dec_chars : 1 + dec_chars;
	__printf_compute_geometry(opt, &g);

	__printf_outn(out, ' ', g.left_spaces);
	if(g.sign) __printf_out(out, g.sign);
	__printf_outn(out, '0', g.zeros);

	int pre = opt->precision;

	if(e >= 0) /* xxxxxx00[.00] */
	{
		/* Decimal dot is after digits; rounding never occurs */
		__printf_outstr(out, digits, length);
		__printf_outn(out, '0', e);

		if(pre > 0)
		{
			__printf_out(out, '.');
			__printf_outn(out, '0', pre);
		}
	}
	else if(length + e > 0) /* xxxy(.xx), xx.xy(xx), xx.xxxx[00] */
	{
		/* Decimal dot is within the digits; we might have rounded */
		__printf_outstr(out, digits, length + e);

		if(pre > 0)
		{
			__printf_out(out, '.');
			__printf_outstr(out, digits + length + e, min(-e,pre));
			__printf_outn(out, '0', pre + e);
		}
	}
	else if(length + e <= 0) /* 0.00(00xxxx), 0.00xy(xx), 0.00xxxx00 */
	{
		/* Decimal dot is before the digits; we might have rounded */
		__printf_out(out, '0');
		if(pre > 0)
		{
			__printf_out(out, '.');
			__printf_outn(out, '0', min(-e - length, pre));
			__printf_outstr(out, digits, min(length,pre+length+e));
			__printf_outn(out, '0', pre + e);
		}
	}

	__printf_outn(out, ' ', g.right_spaces);
}

/* Prints exponent notation for %e and %g. */
static void exponent_notation(
	struct __printf_output *out,
	struct __printf_format *opt,
	struct __printf_geometry g,
	char *digits, int length, int e, int uppercase)
{
	int true_e = e + length - 1;

	/* Number of characters for decimal part and for exponent */
	int dec_chars = opt->precision + (opt->precision > 0);
	int exp_chars = 4 + (true_e >= 100 || true_e <= -100);

	g.content = 1 + dec_chars + exp_chars;
	__printf_compute_geometry(opt, &g);

	__printf_outn(out, ' ', g.left_spaces);
	if(g.sign) __printf_out(out, g.sign);
	__printf_outn(out, '0', g.zeros);

	/* Digits */
	__printf_out(out, digits[0]);
	if(opt->precision > 0)
	{
		__printf_out(out, '.');
		__printf_outstr(out, digits+1, min(length-1, opt->precision));
		__printf_outn(out, '0', opt->precision - (length - 1));
	}

	/* Exponent */
	__printf_out(out, uppercase ? 'E' : 'e');
	__printf_out(out, true_e >= 0 ? '+' : '-');

	if(true_e < 0) true_e = -true_e;
	if(true_e >= 100)
	{
		__printf_out(out, true_e / 100 + '0');
		true_e %= 100;
	}
	__printf_out(out, true_e / 10 + '0');
	__printf_out(out, true_e % 10 + '0');

	__printf_outn(out, ' ', g.right_spaces);
}

//---
// Formatters for kprint
//---

static void __printf_format_eEfFgG(
	struct __printf_output *out,
	struct __printf_format *opt,
	va_list *args)
{
	double v = va_arg(*args, double);
	digit_buffer[0] = '0';
	char *digits = digit_buffer + 1;
	int length = 0, e = 0;

	int is_e = (opt->spec | 0x20) == 'e';
	int is_f = (opt->spec | 0x20) == 'f';
	int is_g = (opt->spec | 0x20) == 'g';

	/* In %e and %f, default to 6 decimals. In %g, default to 6 significant
	   digits, and force to at least 1 */
	if(opt->precision < 0) opt->precision = 6;
	if(opt->precision == 0 && is_g) opt->precision = 1;

	if(special_notation(out, v, (opt->spec & 0x20) == 0)) return;

	/* fabs(v) = int(digits) * 10^e */
	if(v == 0.0) digits[length++] = '0';
	else grisu2(v, digits, &length, &e);
	digits[length] = 0;

	/* In %f and %e, .precision is the number of decimal places; in %g, it
	   is the number of significant digits. Determine the number of decimal
	   places for the rounding (which is one more than the final number if
	   a carry creates a new significant digit on the left) */
	int round_place = opt->precision;
	if(is_e) round_place -= (length + e - 1);
	if(is_g) round_place -= (length + e);

	/* Round off to the specified number of decimal places. digits and
	   length may extend one place left because of carries */
	round_str(&digits, &length, e, round_place);

	struct __printf_geometry g = {
		.sign   = (v < 0) ? '-' : '+',
		.prefix = 0,
		.style  = _PRINTF_NUMERIC,
	};

	if(is_f) {
		return direct_notation(out, opt, g, digits, length, e);
	}
	if(is_e) {
		return exponent_notation(out, opt, g, digits, length, e,
			opt->spec == 'E');
	}

	int true_e = length + e - 1;
	int extreme = (true_e < -4) || (true_e >= opt->precision);

	/* %g is left. Remove decimal zeros at the end of the digit string,
	   starting from the last-shown digit. Keep all figits before the
	   point, the amount of which depends on the mode */
	int removed = remove_zeros(digits, min(length, opt->precision),
		extreme ? 1 : true_e + 1);
	opt->precision -= removed;

	if(extreme)
	{
		/* Don't print more significant digits than we have digits
		   (elimination of trailing zeros) */
		opt->precision = min(opt->precision, length);
		/* There is one leading digit and this many decimal places */
		opt->precision--;

		return exponent_notation(out, opt, g, digits, length, e,
			opt->spec == 'G');
	}
	else
	{
		/* Number of digits before the point */
		int leading_digits = true_e + 1;

		/* Eliminate trailing zeros after the point */
		opt->precision = min(opt->precision, length);
		/* Remove leading digits from decimal place count */
		opt->precision -= leading_digits;

		return direct_notation(out, opt, g, digits, length, e);
	}
}

void __printf_enable_fp(void)
{
	__printf_register('e', __printf_format_eEfFgG);
	__printf_register('E', __printf_format_eEfFgG);
	__printf_register('f', __printf_format_eEfFgG);
	__printf_register('F', __printf_format_eEfFgG);
	__printf_register('g', __printf_format_eEfFgG);
	__printf_register('G', __printf_format_eEfFgG);
}
