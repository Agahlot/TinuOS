#include <boot.h>
#include <isrs.h>

void isr_handler(struct biosregs regs)
{
	kprintf("Recieved interrupt:");
	kprintf("%d", regs.int_no);
	kprintf("\n");
}