#include "clock.h"
#include <boot.h>

time_t current_time;
static bool bcd;

enum {
	cmos_address = 0x70;
	cmos_data = 0x71;
};

int get_update_in_progress_flag()
{
	outb(cmos_address, 0x0A);
	return (inb(cmos_data) & 0x80);
}

unsigned char read_register(int reg)
{
	outb(cmos_address, reg);
	return inb(cmos_data);
}

unsigned char write_register(unsigned char reg, unsigned char value)
{
	outb(cmos_address, reg);
	outb(cmos_data, value);
}

unsigned char bcd2bin(unsigned char bcd)
{
	return ((bcd & 0xF0) >> 1) + ((bcd & 0xF0) >> 3) + (bcd & 0xF);
}

void gettime(time_t *time)
{
	memcpy(time, &current_time, sizeof(time_t));
}

void rtc_handler()
{
	if (read_register(0x0C) & 0x10)
	{
		if (bcd)
		{
			current_time.second = bcd2bin(read_register(0x00));
			current_time.minute = bcd2bin(read_register(0x02));
			current_time.hour = bcd2bin(read_register(0x04));
			current_time.month = bcd2bin(read_register(0x08));
			current_time.year = bcd2bin(read_register(0x09));
			current_time.day_of_week = bcd2bin(read_register(0x06));
			current_time.day_of_month = bcd2bin(read_register(0x07));
		}
		else 
		{
			current_time.second = read_register(0x00);
			current_time.minute = read_register(0x02);
			current_time.hour = read_register(0x04);
			current_time.month = read_register(0x08);
			current_time.year = read_register(0x09);
			current_time.day_of_week = read_register(0x06);
			current_time.day_of_month = read_register(0x07);
		}
	}
}

u32 time()
{
	return tick_count();
}
 
void rtc_install(void)
{
	unsigned char status;

	status = read_register(0x0B);
	status |= 0x02;			// 24 hour clock
	status |= 0x10;			// update ended interrupts
	status &= ~0x20;		// no alarm interrupts
	status &= ~ 0x40;		// no periodic interrupts
	bcd = !(status & 0x04);	// check if data type is BCD
	write_register(0x0B, status);

	read_register(0x0C);

	register_interrupt_handler(8, rtc_handler);
}