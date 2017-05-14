#pragma once

#include <va_list.h>
#include <x86.h>
#include <mmu.h>
#include <spinlock.h>
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof(x[0]))

#define NULL 0

//#define STACK_SIZE	512

/* early_serial_printk.c */
#define IO_BASE		0x3f8

/* Input/Output ports */
static inline void outb(u8 v, u16 port)
{
	/* dN - the d register, Unsigned 8-bit constant */
	asm volatile("outb %0,%1" : :"a" (v), "dN"(port));
}

static inline u8 inb(u16 port)
{
	u8 v;
	asm volatile("inb %1,%0" :"=a" (v) : "dN" (port));
	return v;
}

static inline u32 read_eflags(void)
{
	u32 eflags;
	asm volatile("pushal; popl %0" : "=r" (eflags));
	return eflags;
}

static inline u32 cli(void)
{
	asm volatile("cli");
}

static inline u32 sti(void)
{
	asm volatile("sti");
}

static inline u32 xchg(volatile u32 *addr, u32 newval)
{
	u32 result;
	asm volatile("lock; xchg %0, %1"
				: "+m" (*addr), "=a" (result)
				: "1" (newval)
				: "cc");
	return result;
}

static inline void insl(u32 port, void *addr, int cnt) 
{
    asm volatile (
    	    	    "cld;"
        	 	   	"rep; insl;"
          		    :: "D" (addr), "d" (port), "c" (cnt)
            		: "memory", "cc");
}

/* tty.c */
#define cpu_relax()	asm volatile("rep; nop")

static inline void cr3(u32 p)
{
	asm volatile("movl %0, %%cr3" : : "r"(p));
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

/* biocall for bios interrupt */
struct biosregs {
	unsigned int ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
};

typedef struct biosregs registers_t;
registers_t regs;
typedef void (*isr_t)(registers_t);

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

/* monitor.c */
extern void kprintf(const char* data, ...);
extern void terminal_initialize(void);
extern void terminal_write_hex(u32 n);

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
extern void *memcpy(void *dest, const void *src, size_t n);
extern void * memset(void * b, int val, size_t count);
extern char *strcpy(char *dest, const char *src);
extern char *strncpy(char *dest, const char *src, size_t n);

/* gdt */
void gdt(void);
extern void gdt_flush(u32);
extern void tss_flush();
extern void set_kernel_stack(uintptr_t stack);

/* idt */
void idt(void);
extern void set_idt(u32 num, u32 offset, u16 selector, u8 flags);
extern void idt_flush();

/* irq */
extern void irq_remap();
extern void set_irq();
extern void irq_handler();

/* isrs.c */
extern void isr_handler(registers_t regs);
extern void register_interrupt_handler(u8, isr_t handler);
extern void unregister_interrupt_handler(u8);

/* timer.c */
extern void init_timer(u32 frequency);
extern u32 tick_count();
extern void sleep(u32 ticks);

/* spinlock.c */
extern void initlock(lock *lk, char *name);
extern void acquire(lock *lk);
extern void release(lock *lk);
extern void pushcli(void);
extern int holding(lock *lk);

/* kmalloc.c */
extern void kinit1(void *vstart, void *vend);
extern void kinit2(void *vstart, void *vend);
extern void freerange(void *vstart, void *vend);
extern char *kalloc(void);
extern void kfree(char *);

/* mmu.c */
extern void paging_install(void);
extern u32 *pagetable_init(void);
extern u32 mappages(u32 *pgdir, void *va, u32 size, u32 pa, int perm);
extern u32 *pagewalk(u32 *pgdir, void *va, int alloc);
extern void page_fault(void);

/* umalloc.c */
extern void* malloc(u32 size);
extern void free(void *ap);
extern void *ksbrk(int allocate);


/* die */
void __attribute__((noreturn)) die(void);

/* memory.c */
int detect_memory(void);

/* apm.c */
int query_apm_bios(void);

/* edd.c */
void query_edd(void);