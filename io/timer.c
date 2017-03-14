#include <boot.h>

u32 tick = 0;

static void timer_callback(registers_t regs)
{
	tick++;
	kprintf("Tick: %d\n", tick);
}

void init_timer(u32 frequency)
{
	register_interrupt_handler(32, &timer_callback);
	u32 divisor = 1193180/frequency;

	/* Send command word to the PIT
	 * 110110b - Sets binary counting, Mode 3, Read LSB
	 * first then MSB, Channel 0.
	 */ 
	outb(0x36, 0x40);
	u8 l = (u8)(divisor & 0xFF);
	u8 h = (u8)((divisor>>8) & 0xFF);

	/* Because we have set "Load LSB first and then MSB"
	 * so we send low byte and then high byte.
	 */
	outb(l, 0x40);
	outb(h, 0x40);

}