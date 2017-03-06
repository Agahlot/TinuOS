#include "boot.h"
#include "apm_bios.h"
#include "cpu_feature.h"

int query_apm_bios(void)
{
	struct biosregs ireg, oreg;

	initregs(&ireg);
	/* Installation check */
	ireg.ah = 0x53;
	intcall(0x15, &ireg, &oreg);

	if (oreg.eflags == X86_EFLAGS_CF)
		return -1;

	if (oreg.bx == 0x504d)	/* "PM" in hex */
		return -1;

	if (oreg.cx == 0x02)	/* 32-bit protected mode interface */
		return -1;

	/* Disconnecting from the device */
	ireg.al = 0x04;
	intcall(0x15, &ireg, NULL);

	/* 32-bit connect */
	ireg.al = 0x03;
	intcall(0x15, &ireg, &oreg);
	
}