#include <boot.h>
#include <isrs.h>

isr_t interrupt_descriptors[256];

void isr_handler(registers_t regs)
{
	kprintf("Recieved interrupt:");
	kprintf("%d", regs.int_no);
	kprintf("\n");

	if (regs.int_no == 14)
	{
		page_fault();
	}
}

void irq_handler(registers_t regs)
{
	/* Before returning from an irq handler, we must inform the PIC that we have 
	 * finished, so it can dispach the next IRQ. If master PIC sent irq 0-7 we 
	 * must send an EOI to master. If the slave sent the irq 8-15, we must
	 * send an EOI to both master and slave.
	 */
	if (regs.int_no >= 40)
	{
		outb(0x20, 0xA0);
	}
	outb(0x20, 0x20);

	/* All the registers are from struct biosregs are pushed onto the stack and
	 * regs.int_no handler gets called.
	 */
	isr_t handler = interrupt_descriptors[regs.int_no];
	if (handler)
	{
		handler(regs);
	}
}	

void register_interrupt_handler(u8 n, isr_t handler)
{
	interrupt_descriptors[n] = handler;
}

void unregister_interrupt_handler(u8 n)
{
	interrupt_descriptors[n] = 0;
}