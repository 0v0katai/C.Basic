#include <fxlibc/printf.h>

void __printf_compute_geometry(
	struct __printf_format *opt,
	struct __printf_geometry *g)
{
	int integral = (g->style == _PRINTF_INTEGER);
	int numerical = (g->style == _PRINTF_NUMERIC) || integral;
	int padding;

	/* Sign character (no discussion required for negative values) */
	if(numerical && g->sign == '+')
	{
		g->sign = 0;
		if(opt->blank_sign) g->sign = ' ';
		if(opt->force_sign) g->sign = '+';
	}

	g->zeros = 0;

	padding = opt->length - (g->sign != 0) - g->prefix
		- (g->content > opt->precision ? g->content : opt->precision);
	if(padding < 0) padding = 0;

	/* In integral modes, precision forces zeros */
	if(integral && opt->precision >= 0)
	{
		if(opt->alignment == '0') opt->alignment = 0;

		int zeros = opt->precision - g->content;
		if(zeros > 0) g->zeros = zeros;
	}

	if(opt->alignment == '0')
	{
		/* Zeros are only allowed in numerical modes */
		if(numerical) g->zeros = padding;
		else g->left_spaces = padding;
	}
	else if(opt->alignment == '-')
	{
		g->right_spaces = padding;
	}
	else
	{
		g->left_spaces = padding;
	}
}

int64_t __printf_load_i(int size, va_list *args)
{
	if(size == 1)
		return (int8_t)va_arg(*args, int);
	if(size == 2)
		return (int16_t)va_arg(*args, int);
	if(size == 8)
		return va_arg(*args, long long);

	return va_arg(*args, int);
}

uint64_t __printf_load_u(int size, va_list *args)
{
	if(size == 1)
		return (uint8_t)va_arg(*args, unsigned int);
	if(size == 2)
		return (uint16_t)va_arg(*args, unsigned int);
	if(size == 8)
		return va_arg(*args, unsigned long long);

	return va_arg(*args, unsigned int);
}

int __printf_digits10(char *str, uint64_t n)
{
	int digits = 0;
	while(n || !digits)
	{
		/* TODO: Use fast division in __printf_digits10() */
		str[digits++] = (n % 10) + '0';
		n /= 10;
	}
	return digits;
}

int __printf_digits16(char *str, int uppercase, uint64_t n)
{
	char *hex = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
	int digits = 0;

	while(n || !digits)
	{
		str[digits++] = hex[n & 0xf];
		n >>= 4;
	}
	return digits;
}

int __printf_digits8(char *str, uint64_t n)
{
	int digits = 0;

	while(n || !digits)
	{
		str[digits++] = (n & 0x7) + '0';
		n >>= 3;
	}
	return digits;
}
