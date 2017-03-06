#include "boot.h"
#include "string.h"

int sprintf(char *buf, const char *format, ...)
{
	va_list ap;
	int i;

	va_start(ap, format);
	i = vasprintf(buf, format, ap);
	va_end(ap);
	return i;
}

int printf(const char *fmt, ...)
{
	char buf[1024];
	int printed;

	va_list ap;
	va_start(ap, fmt);
	printed = vasprintf(buf, fmt, ap);
	va_end(ap);

	puts(buf);

	return printed;
}