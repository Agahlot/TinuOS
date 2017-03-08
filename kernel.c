#include <boot.h>
#include <multiboot.h>

int kernel_main(unsigned long magic, unsigned long addr)
{
    /* setup gdt */
	gdt();
	idt();
	terminal_initialize();
	kprintf("hello, world!\n");

	/* setup idt */
	asm volatile("int $0x3");
	asm volatile("int $0x4");	
	return 0;
}