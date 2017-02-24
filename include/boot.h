#ifndef BOOT_BOOT_H
#define BOOT_BOOT_H

#include "types.h"
#include "va_list.h"
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

/* early_serial_printk.c */
#define IO_BASE		0x3f8

/* Input/Output ports */
static inline void outb(u8 v, u16 port)
{
	/* dN - the d register, Unsigned 8-bit constant */
	asm volatile("outb %0,%1" : :"a" (v), "dN"(port));
}

static inline void inb(u16 port)
{
	u8 v;
	asm volatile("inb %1,%0" :"=a" (v) : "dN" (port));
}

/* Defined in bootparam.h */
extern struct setup_header hdr;
extern struct boot_params boot_params;

/* The two !! is this macro will first convert a 0 to 0
 * and any non-zero value to 1. If the x is 0(false) we 
 * get 1 and if x is 1(false) we get -1, which results in 
 * error as sizeof array cannot be negative
 */
#define BUILD_BUG_ON(x)		((void)sizeof(char[1-2*!!(x)]))

/* early_serial_printk.c */
void console_init(void);

/* printk.c */
extern size_t vasprintf(char *buf, const char *fmt, va_list args);


#endif /* BOOT_BOOT_H */