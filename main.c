#include "boot.h"
#include "bootparam.h"
#include "string.h"

char *heap_end = _end;

struct boot_params boot_params __attribute__((aligned(16)));

/* Copy bootparams from hdr in header.S to setup_header */

static void copy_boot_params(void)
{
	BUILD_BUG_ON(sizeof boot_params != 4096);
	memcpy(&boot_params.hdr, &hdr, sizeof hdr);
}

static void init_heap(void)
{
	char *stack_end;

	if (boot_params.hdr.loadflags & CAN_USE_HEAP)
	{
		/* Without "P" --> leal $-512(%esp), %eax
		 * With "P"    --> leal  -512(%esp), %eax (This is what we want!)
		 * "i" --> immediate integer operand(one with constant value)
		 */

		asm("leal %P1(%%esp),%0" : "=r" (stack_end) : "i" (-STACK_SIZE));
		heap_end = (char*)(size_t)(boot_params.hdr.heap_end_ptr + 0x200);

		if (heap_end > stack_end) {
			stack_end = heap_end;
		}
		/* NO heap */
		else {
			puts("WARNING: Ancient bootloader, some functionality "
				 "may be limited!\n");
		}
		
	}
}

static void keyboard_init(void)
{
	struct biosregs ireg, oreg;
	initregs(&ireg);

	ireg.ah = 0x02;	/* Get Keyboard status */
	intcall(0x16, &ireg, &oreg);
	boot_params.kbd_status = oreg.al; /* AL = shift flags */

	ireg.ax = 0x0305;	/* Set keyboard repeat rate */
	intcall(0x16, &ireg, &oreg);
}

int main()
{
	/* So called zero page */
	copy_boot_params();

	/* Initialize earlyprintk */
	console_init();
		puts("Initialize early console")
		kprintf(LOGLEVEL_NOTICE, "This is debug print");

	/* Initialize heap */
	init_heap();

	/*
	 * Enable long mode 
	 * set_bios_mode();
	 */

	/* Detect memory layout */
	detect_memory();

	/* Initialize Keyboard */ 
	keyboard_init();

	/*TODO Advance Power Management */
	query_apm_bios();

	/*TODO query edd information */
	query_edd();
}
