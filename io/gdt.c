#include <boot.h>
#include <x86.h>

tss_entry_t tss_entry;

struct gdt_entry {
		unsigned short limit_low;
		unsigned short base_low;
		unsigned char base_middle;
		unsigned char access;
		unsigned char granularity;
		unsigned char base_high;
} __attribute__((packed));

/* GDT pointer which points to our gdt structure */
struct gdt_ptr {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

typedef struct gdt_entry gdt_entry_t;
typedef struct gdt_ptr gdt_ptr_t;

gdt_entry_t gp[6];
gdt_ptr_t gdpt;

void set_gdt(u32 num, u32 base, u32 limit, u8 access, u8 gran)
{
	gp[num].base_low = (base & 0xFFFF);
	gp[num].base_middle = (base >> 16) & 0xFF;
	gp[num].base_high = (base >> 24) & 0xFF;

	gp[num].limit_low = (limit & 0xFFFF);

	/* granularity bit is from 16-24 */

	gp[num].granularity = (limit >> 16) & 0x0F;

	gp[num].granularity |= (gran & 0xF0);
	gp[num].access = access;
}

void set_tss(u32 num, u16 ss0, u32 esp0)
{
	uintptr_t base;
	uintptr_t limit;

	/* address of tss */
	base = (uintptr_t)&tss_entry;
	limit = base + sizeof(tss_entry);

	/* Add tss descriptor to gdt */
	set_gdt(num, 4096, 103, 0x89, 0x00);

	memset(&tss_entry, 0, sizeof(tss_entry_t));

	tss_entry.ss0 = ss0;
	tss_entry.esp0 = esp0;

	tss_entry.cs     = 0x08;
	tss_entry.ss     = 0x10;
		tss_entry.ds = 0x10;
		tss_entry.es = 0x10;
		tss_entry.fs = 0x10;
		tss_entry.gs = 0x10;
	tss_entry.iomap_base = sizeof(tss_entry_t);
}

void gdt(void)
{
	gdpt.limit = (sizeof(gdt_entry_t)*6)-1;
	gdpt.base = (uintptr_t)&gp;

	set_gdt(0, 0, 0, 0, 0);
	set_gdt(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* kernel code segment */
	set_gdt(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* kernel data segment */
	set_gdt(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* User mode code segment */
    set_gdt(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* User mode data segment */

	//set_tss(5, 0x10, 0);

	gdt_flush((uintptr_t)&gdpt);
	//tss_flush();
}

void set_kernel_stack(uintptr_t stack)
{
	tss_entry.esp0 = stack;
}