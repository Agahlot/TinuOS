#include "va_list.h"
#include "kern_level.h"
#include "boot.h"

unsigned short *textmem = (unsigned short *)0xB800;
/* Current position = x + y * 80. */
void putchar(unsigned char ch, int x, int y, unsigned attr)
{
	unsigned short *where;
	unsigned attrib = attr << 8;
	where = textmem + (x + y * 80);
	*where = ch | attrib;
}

static void print_dec(unsigned int va, unsigned int arg_width, char * buf, int * ptr ) 
{
	unsigned int n_width = 1;
	unsigned int i = 9;
	while (va > i && i < UINT32_MAX) {
		n_width += 1;
		i *= 10;
		i += 9;
	}

	int printed = 0;
	while (n_width + printed < arg_width) {
		buf[*ptr] = '0';
		*ptr += 1;
		printed += 1;
	}

	i = n_width;
	while (i > 0) {
		unsigned int n = va / 10;
		int r = va % 10;
		buf[*ptr + i - 1] = r + '0';
		i--;
		va = n;
	}
	*ptr += n_width;
}


static void print_hex(unsigned long va, int arg_width, char *buf, int * ptr)
{
	int i = arg_width;

	if (i == 0) i = 8;

	unsigned int n_width = 1;
	unsigned int j = 0x0F;
	while (va > j && j < UINT32_MAX) {
		n_width += 1;
		j *= 0x10;
		j += 0x0F;
	}

	while (i > (int)n_width) {
		buf[*ptr] = '0';
		*ptr += 1;
		i--;
	}

	i = (int)n_width;
	while (i-- > 0) {
		buf[*ptr] = "0123456789abcdef"[(va>>(i*4))&0xF];
		*ptr += + 1;
	}
}

size_t vasprintf(char *buf, const char *fmt, va_list args)
{
	int i=0;
	int ptr=0;
	char *s;
	int len = strlen(fmt)
	for (; i < len; i++)
	{
		if (fmt[i] != '%') {
			buf[ptr++] = fmt[i];
			continue;
		}
		++i;

		unsigned int arg_width = 0;
		while(fmt[i] >= '0' && fmt[i] <= '9')
		{
			arg_width += fmt[i] - '0';
			++i;
		}

		/*fmt[i] == % */
		switch(fmt[i])
		{
			case 's':
				s = (char *)va_arg(args, char *);
				while (*s)
				{
					buf[ptr++] = *s++;
				}
				break;

			case 'c':
				buf[ptr++] = (char)va_arg(args, int);
				break;
			
			case 'x': /* Hexadecimal character */
				print_hex((unsigned long)va_arg(args, unsigned long), arg_width, buf, &ptr);
				break;

			case 'd':
				print_dec((unsigned int)va_arg(args, unsigned int), arg_width, buf, &ptr);
				break;

			case '%':
				buf[ptr++] = '%';
				break;

			default:
				buf[ptr++] = fmt[i];
				break;			
		}
	}
	/* Buffer ends with a NULL */
	buf[ptr] = '\0';
	return ptr;
}

static void itoa(char *buf, int base, int d)
{
	char *p = buf;
	char *start, *end;
	unsigned long ud = d;
	int divisor = 10;

	/* Negative number is in base 10 */
	if (base == 'd' && d < 0)
	{
		*p[0] = '-';
		buf++;
		ud = -d;
	}
	else if (base == 'x')
	{
		divisor = 16;
	}
	do {
		int remainder = ud % divisor;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	}while (ud /= divisor);

	/* terminate buf */
	*p = 0;

	/* reverse buf */
	start = buf;
	end = p - 1;
	while (start < end)
	{
		char *temp = start;
		start = end;
		end = start;
		start++;
		end--;
	}
}

int kprintf(char *fmt, ...)
{
	char buf[1024];
	int y, x;
	va_list args;
	va_start(args, fmt);
	int out = vasprintf(buf, fmt, args);
	va_end(args);

	serial_print(IO_BASE, buf);

	int temp = 0xFFFF; /* Upper FF-row, Lower FF-column */
	if (1)
	{
		/* Cursor location low byte in Index register */
		outb(0x0F, 0x3D4);
		outb(temp, 0x3D5);
		/* Cursor location high byte in Index register */
		outb(0x0E, 0x3D4);
		outb(temp >> 8, 0x3D5);

		for (y=0; y<80; y++)
		{
			for (x=0; x<24; x++)
			{
				putchar(' ', x, y, 0x00);
			}
		}
	}
	char **arg = (char **)buf;
	int c;
	char buf[20];

	arg++;
	while ((c = *buf++) != 0)
	{
		if (c != '%')
		{
			putchar(c, x, y, 0x2);
		}
		else if (c == '\n')
		{
			x = 0;
			y +=1;
		}
		else
		{
			char *p;
			c = *data++;
			switch(c)
			{
				case 'd':
				case 'u':
				case 'x':
					itoa(buf, c, *((int *)arg++));
					p = buf;
					goto string;
					break;

				case 's':
					p = *arg++;
					if (!p)
						p = "(null)";

				string:
					while (*p)
						putchar(*p++, x, y, 0x2);
					break;
					
				default:
					putchar(*((int *)arg++), x, y, 0x2);
				break;						
			}
		}

	}	
}

log_level level = NORMAL;

static char *c_messages[] = {
		LOGLEVEL_DEFAULT=1,
		LOGLEVEL_EMERG,
		LOGLEVEL_ALERT,
		LOGLEVEL_ERR,
		LOGLEVEL_WARINIG,
		LOGLEVEL_NOTICE,
		LOGLEVEL_DEBUG,
};

void debug_print(char *title, int line_no, log_level level, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	char buf[1024];
	vasprintf(buf, fmt, args);

	va_end(args);
	kprintf("%10d.%2d:%s:%d  %s:%s\n", timer_ticks, timer_subticks, title, line_no, c_messages[level], buffer);
}