/* Serial i/o using bios interrupt */

#include "boot.h"

#define XMTRDY	0x20

#define TXR		0
#define LSR		5	/* Line status register */

static void __attribute__((section(".inttext"))) serial_putchar(char ch)
{
	unsigned timeout = 0xffff;

	while ((inb(0x3f8 + LSR) & XMTRDY) == 0 && --timeout)
			cpu_relax();

	outb(ch, 0x3f8 + TXR);
}

static void __attribute__((section(".inttext"))) biosputchar(char ch)
{
	struct biosregs regs;
	initregs(&regs);
	regs.bx = 0x0007;
	regs.cx = 0x0001;
	regs.ah = 0x0e;
	regs.al = ch;
	intcall(0x10, &regs, NULL);
}

static void __attribute__((section(".inttext"))) putchar(char ch)
{
	/* If \n then add \r ---> \r\n */
	if (ch == '\n')
		putchar('\r');

	biosputchar(ch);

	/* Send the character to the serial port */
	serial_putchar(ch);
}

void __attribute__((section(."inttext"))) puts(char *str)
{
	while (*str)
	{
		putchar(*str);
		str++;
	}
}