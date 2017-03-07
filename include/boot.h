#ifndef BOOT_BOOT_H
#define BOOT_BOOT_H

#include "types.h"
#include "va_list.h"
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

//#define STACK_SIZE	512

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
/*
static inline u16 ds(void)
{
	u16 seg;
	asm volatile("movw %%ds,%0" :"=rm" (seg));
	return seg;
}

static inline u16 fs(void)
{
	u16 seg;
	asm volatile("movw %%fs,%0", :"=rm" (seg));
	return seg;
}

static inline u16 gs(void)
{
	u16 seg;
	asm volatile("movw %%gs,%0", :"=rm" (seg));
	return seg;
}*/

/* tty.c */
#define cpu_relax()	asm volatile("rep; nop")

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

/* biocall for bios interrupt */
struct biosregs {
	union {
		struct {
			u32 edi;
			u32 esi;
			u32 ebp;
			u32 _esp;
			u32 ebx;
			u32 edx;
			u32 ecx;
			u32 eax;
			u32 _fsgs;
			u32 _dses;
			u32 eflags;
		};
		struct {
			u16 di;
			u16 si;
			u16 bp;
			u16 _sp;
			u16 bx;
			u16 dx;
			u16 cx;
			u16 ax;
			u16 gs, fs;
			u16 es, ds;
			u16 flags;
		};
		struct {
			u8 al, ah;
			u8 bl, bh;
			u8 cl, ch;
			u8 dl, dh;
		};
	};
};

typedef struct tss_entry {
	u32 prev_tss;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u16 trap;
	u16 iomap_base;
} __attribute__((packed)) tss_entry_t;

void intcall(u8 int_no, const struct biosregs *regs, struct biosregs *oregs);

/* regs.c */
void initregs(struct biosregs *regs);

/* cpucheck.c */
int validate_cpu();
int do_intel(int *cpu_level_ptr, int *req_level_ptr);

/* printf.c */
int sprintf(char *buf, const char *format, ...);
int vsprintf(char *str, const char *format, va_list ap);
int printf(const char *fmt, ...);

/* tty.c */
void puts(char *str);
void putchar(char);

/* copy functions */
extern void memset(void *s, int c, size_t n);
extern void memcpy(void *dest, const void *src, size_t n);

/* gdt */
void gdt(void);
extern void gdt_flush();
extern void tss_flush();
extern void set_kernel_stack(unsigned long stack);

/* idt */
extern void load_idt();

/* memory.c */
int detect_memory(void);

/* apm.c */
int query_apm_bios(void);

/* edd.c */
void query_edd(void);

#endif /* BOOT_BOOT_H */