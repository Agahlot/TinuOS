#include <boot.h>
#include <multiboot.h>

int kernel_main(unsigned long magic, unsigned long addr)
{
    /* setup gdt */
	gdt();
	idt();
	terminal_initialize();
	kprintf("Hello, World!\n");

	/* setup idt */
	asm volatile("int $0x3");
	asm volatile("int $0x4");

	asm volatile("sti");
	init_timer(50);	
	return 0;
}