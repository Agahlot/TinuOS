#include "boot.h"

/* DLAB=0 registers */
#define RBR_REG		(IO_BASE + 0)
#define THR_REG		(IO_BASE + 0)
#define IER_REG		(IO_BASE + 1)

/* DLAB=1 registers */
#define LS_REG		(IO_BASE + 0)	/* DLL divisor latch for LSB */
#define MS_REG		(IO_BASE + 1)	/* DLM divisor latch for MSB */

/* DLAB-insensitive registers */
#define IIR_REG		(IO_BASE + 2)	/* interrupt identification */
#define FCR_REG		(IO_BASE + 2)	/* FIFO control */
#define LCR_REG		(IO_BASE + 3)	/* line control register */
#define MCR_REG		(IO_BASE + 4)	/* modem control */
#define LSR_REG		(IO_BASE + 5)	/* line status */

/* Interrupt Enable register bits */
#define IER_RDA		0				/* IER recieved data available */
#define IER_THR		1				/* transmitter holding register */
#define IER_RLSR	2				/* reciever line status */

/* Line control register bits */
#define LCR_N81		0x03			/* No parity, 8 data bits, 1 stop bit */
#define LCR_DLAB	0x80			/* DLL and DLM accessible bit */

/* Modem control register */
#define MCR_OUT2	0x08 			/* MCR output 2 */

/* Line status register */
#define LSR_DA		0				/* data available */
#define LSR_THR		5				/* THR is empty */

void serial_enable()
{
	outb(0x00, IER_REG); /* Interrupt disable */
	outb(0x80, LCR_REG); /* Enable DLAB */
	outb(0x03, LS_REG);  
	outb(0x00, MS_REG);
	outb(0x03, LCR_REG);
	outb(0xC7, IIR_REG);
	outb(0x0B, MCR_REG);
}

void console_init(void)
{
	serial_enable();
}

int early_serial_putc()
{
	return inb(LS_REG) & 0x20;
}

void early_serial_write(char ch)
{
	while (early_serial_putc() == 0);

	outb(IO_BASE, ch);
}

void serial_print(unsigned int port, char * buf)
{
	for (i=0; i<strlen(buf); i++)
	{
		early_serial_write(buf[i]);
	}
}