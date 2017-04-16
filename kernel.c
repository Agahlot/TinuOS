#include <x86.h>
#include <boot.h>
#include <memlayout.h>
#include <multiboot.h>
#include <paging.h>

extern char end[];
u32 entrypgdir[];

int kernel_main()
{
    /* setup gdt */
	gdt();
	idt();
	kinit1(&end, P2V(4*1024*1024));
	terminal_initialize();
	kprintf("HELLO");
	paging_install();
	return 0;
}

/* Every Page Directory is page size aligned */

__attribute__((__aligned__(PAGE_SIZE)))
u32 entrypgdir[1024] = {
	/* map VA[0 - 4MB] to PA[0 - 4MB] */
	[0] = (0) | PTE_P | PTE_RW | PDE_PS,
	/* map VA[KERNBASE - KERNBASE+4MB] to PA[0 - 4MB] */
	[KERNBASE>>PDE_SHIFT] = (0) | PTE_P | PTE_RW | PDE_PS,
};