#include <boot.h>
#include "../drivers/rtc/clock.h"

static u32 tick = 0;

static int pit_handler_nest()
{
	return 0;
}


/* By default, the timer fires 18.222 times per second. Why 18.222Hz? 
 * Some engineer at IBM must've been smoking something funky. 
 */
static void timer_callback(registers_t regs)
{
	tick++;
	if (tick % 19 == 0)
	{
		if (pit_handler_nest() != 0)
		{
			kprintf("PIT handler error");
		}
	}

}

u32 tick_count()
{
	return tick;
}

/* TODO */
void sleep(u32 millisecond)
{
	u32 end = tick_count() + millisecond;
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