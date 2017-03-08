#include <boot.h>
#include <multiboot.h>

int kernel_main(unsigned long magic, unsigned long addr)
{
    /* setup gdt */
	//gdt();
	terminal_initialize();
	kprintf("hello, world!");

	/* setup idt */
	//idt();
	//asm volatile("int $0x3");
	//asm volatile("int $0x4");	
	return 0;
}