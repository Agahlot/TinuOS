#include <boot.h>
#include <isrs.h>

void isr_handler(struct biosregs regs)
{
	kprintf("Revieved interrupt:");
	kprintf("%d", regs.int_no);
	kprintf("\n");
}