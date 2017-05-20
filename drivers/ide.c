#include "ide.h"
#include <boot.h>

struct IDEChannelRegisters {
	unsigned short base;	// I/O base
	unsigned short ctrl;	// Control Base
	unsigned short bmide;	// Bus Master IDE
	unsigned char nIEN;		// nIEN (No Interrupt)
} channels[2];

/* Buffer to read identification space into */
unsigned char ide_buf[2048] = { 0 };
unsigned static char ide_irq_invoked = 0;
unsigned static char atapi_packet[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

struct ide_device {
	unsigned char reserved;
	unsigned char channel;
	unsigned char drive;
	unsigned short type;
	unsigned short sign;
	unsigned short capabilities;
	unsigned int commandsets;
	unsigned int size;
	unsigned char model[41];
} ide_devices[4];		// IDE can contain 4 devices

void ide_write(unsigned char channel, unsigned char reg, unsigned char data)
{
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);

	if (reg < 0x08)
		outb(data, channels[channel].base + reg - 0x00);

	else if (reg < 0x0C)
		outb(data, channels[channel].base + reg - 0x06);
	else if (reg < 0x0E)
		outb(data, channels[channel].ctrl + reg - 0x0A);
	else if (reg < 0x16)
		outb(data, channels[channel].bmide + reg - 0x0E);

	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

unsigned char ide_read(unsigned char channel, unsigned char reg)
{
	unsigned char result;
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);

	if (reg < 0x08)
		result = inb(channels[channel].base + reg - 0x00);

	else if (reg < 0x0C)
		result = inb(channels[channel].base + reg - 0x06);
	else if (reg < 0x0E)
		result = inb(channels[channel].ctrl + reg - 0x0A);
	else if (reg < 0x16)
		result = inb(channels[channel].bmide + reg - 0x0E);

	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);

	return result;
}

void ide_read_buffer(unsigned char channel, unsigned char reg, unsigned int buffer, unsigned int quads)
{
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);

	asm("pushw %es; movw %ds, %ax; movw %ax, %es");

	if (reg < 0x08)
		insl(channels[channel].base + reg - 0x00, buffer, quads);

	else if (reg < 0x0C)
		insl(channels[channel].base + reg - 0x06, buffer, quads);
	else if (reg < 0x0E)
		insl(channels[channel].ctrl + reg - 0x0A, buffer, quads);
	else if (reg < 0x16)
		insl(channels[channel].bmide + reg - 0x0E, buffer, quads);

	asm("popw %es;");

	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

unsigned char ide_polling(unsigned char channel, unsigned int advanced_check)
{
	/* Delay 400ns for BSY to be set */
	ide_read(channel, ATA_REG_ALTSTATUS);
	ide_read(channel, ATA_REG_ALTSTATUS);
	ide_read(channel, ATA_REG_ALTSTATUS);
	ide_read(channel, ATA_REG_ALTSTATUS);

	/* Wait until BSY bit is 0 */
	while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY); 

	if (advanced_check)
	{
		unsigned char state = ide_read(channel, ATA_REG_STATUS);

		/* Error checking */
		if (state & ATA_SR_ERR)
			return 2;

		/* Drive write fault */
		if (state & ATA_SR_DF)
			return 1;

		/* Data request ready */
		if (!(state & ATA_SR_DRQ))
			return 3;
	}
	/* NO ERROR! */
	return 0;
}

/* Error function */
unsigned char ide_print_error(unsigned int drive, unsigned char err)
{
	if (err == 0)
		return err;

	kprintf(" IDE:");
	if (err == 1)
	{
		kprintf("- Device Fault\n");
		err = 19;
	}
	else if (err == 2)
	{
		unsigned char st = ide_read(ide_devices[drive].channel, ATA_REG_ERROR);
		if (st & ATA_ER_AMNF)
		{
			kprintf("- No Address Mark Found\n");
			err = 7;
		}
		if (st & ATA_ER_TK0NF)
		{
			kprintf("- No Media or Media Error\n");
			err = 3;
		}
		if (st & ATA_ER_ABRT)
		{
			kprintf("- Command Aborted\n");
			err = 20;
		}
		if (st & ATA_ER_MCR)
		{
			kprintf("- No Media or Media Error\n");
			err = 3;
		}
		if (st & ATA_ER_IDNF)
		{
			kprintf("- ID Mark Not Found\n");
			err = 21;
		}
		if (st & ATA_ER_MC)
		{
			kprintf("- No Media or Media Error\n");
			err = 3;
		}
		if (st & ATA_ER_UNC)
		{
			kprintf("- Uncorrectable Data Error\n");
			err = 22;
		}
		if (st & ATA_ER_BBK)
		{
			kprintf("- Bad Sectors\n");
			err = 13;
		}
	}
	else if (err == 3)
	{
		kprintf("- Reads Nothing\n");
		err = 23;
	}
	else if (err == 4)
	{
		kprintf("- Write Protected\n");;
		err = 8;
	}

	kprintf("- [%s %s] %s\n",
		(const char *[]){"Primary", "Secondary"}[ide_devices[drive].channel],
		(const char *[]){"Master", "Slave"}[ide_devices[drive].drive],
		ide_devices[drive].model);

	return err;
}

void ide_initialize(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4) 
{

   int j, k, count = 0;

   /* 1- Detect I/O Ports which interface IDE Controller: */
   channels[ATA_PRIMARY  ].base  = (BAR0 &= 0xFFFFFFFC) + 0x1F0*(!BAR0);
   channels[ATA_PRIMARY  ].ctrl  = (BAR1 &= 0xFFFFFFFC) + 0x3F4*(!BAR1);
   channels[ATA_SECONDARY].base  = (BAR2 &= 0xFFFFFFFC) + 0x170*(!BAR2);
   channels[ATA_SECONDARY].ctrl  = (BAR3 &= 0xFFFFFFFC) + 0x374*(!BAR3);
   channels[ATA_PRIMARY  ].bmide = (BAR4 &= 0xFFFFFFFC) + 0; // Bus Master IDE
   channels[ATA_SECONDARY].bmide = (BAR4 &= 0xFFFFFFFC) + 8; // Bus Master IDE

   /* 2- Disable IRQs: */
   ide_write(ATA_PRIMARY, ATA_REG_CONTROL, 2);
   ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

   
}