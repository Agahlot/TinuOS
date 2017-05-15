#include "clock.h"
#include <boot.h>
#include <stdbool.h>

typedef struct {
	char str[200];
} str_t;

static char* dates_str[7] =
{
	"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" 
};

static char* months_str[12] = 
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
	"Aug", "Sept", "Oct", "Nov", "Dec"
}; 

time_t current_time;
static bool bcd;

enum {
	cmos_address = 0x70,
	cmos_data = 0x71,
};

static str_t date_to_text(u8 num)
{
	str_t date;
	if (num < 1 || num > 7)
		kprintf("Invalid Week");
	strcpy(date.str, dates_str[num-2]);
	return date;
}

static str_t month_to_text(u8 num)
{
	str_t month;
	if (num < 1 || num > 12) {
		kprintf("Invalid Month");
	}
	strcpy(month.str, months_str[num-1]);
	return month;
}

u8 read_register(int reg)
{
	outb(reg, cmos_address);
	return inb(cmos_data);
}

int get_update_in_progress_flag() 
{
	outb(0x0A, cmos_address);
	return (inb(cmos_data) & 0x80);
}

unsigned char write_register(unsigned char reg, unsigned char value)
{
	outb(reg, cmos_address);
	outb(value, cmos_data);
}

unsigned char bcd2bin(unsigned char bcd)
{
	return ((bcd & 0xF0) >> 1) + ((bcd & 0xF0) >> 3) + (bcd & 0xF);
}

void gettime(time_t *time)
{
	memcpy(time, &current_time, sizeof(time_t));
}

static void rtc_handler(registers_t regs)
{
	bool ready = read_register(0x0C) & 0x10;
	if (ready)
	{
	/*	if (bcd)
		{
			current_time.second = bcd2bin(read_register(0x00));
			current_time.minute = bcd2bin(read_register(0x02));
			current_time.hour = bcd2bin(read_register(0x04));
			current_time.month = bcd2bin(read_register(0x08));
			current_time.year = bcd2bin(read_register(0x09));
			current_time.day_of_week = bcd2bin(read_register(0x06));
			current_time.day_of_month = bcd2bin(read_register(0x07));
			current_time.century = bcd2bin(read_register(0x32));
		}
		else 
	*/	
		{
			current_time.second = read_register(0x00);
			current_time.minute = read_register(0x02);
			current_time.hour = read_register(0x04);
			current_time.month = read_register(0x08);
			current_time.year = read_register(0x09);
			current_time.day_of_week = read_register(0x06);
			current_time.day_of_month = read_register(0x07);
			current_time.century = read_register(0x32);
		}
	}	
	str_t weekday = date_to_text(current_time.day_of_week);
	str_t month = month_to_text(current_time.month);
	kprintf("%s %s %x:%x:%x UTC %x%x\n",
				(weekday.str),
				(month.str),
				current_time.hour,
				current_time.minute,
				current_time.second,
				current_time.century,
				current_time.year);
}

void rtc_print_struct(time_t current_time)
{
	str_t weekday = date_to_text(current_time.day_of_week);
	str_t month = month_to_text(current_time.day_of_month);
	kprintf("%s %s %x:%x:%x UTC %x%x\n",
				(weekday.str),
				(month.str),
				current_time.hour,
				current_time.minute,
				current_time.second,
				current_time.century,
				current_time.year);
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

	register_interrupt_handler(40, &rtc_handler);
}
