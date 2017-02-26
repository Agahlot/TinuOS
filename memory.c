#include "boot.h"

#define SMAP	0x534d4150
#define X86_EFLAGS_CF	1

static int detect_memory_e820(void)
{
	int count;
	struct biosregs ireg, oreg;
	struct biosregs *desc = boot_params.e820_map
	struct e820entry buf;

	initregs(&ireg);
	ireg.ax = 0xe820;
	ireg.cx = sizeof buf;
	ireg.edx = SMAP;
	ireg.di = (size_t)&buf;

	do {
	 	intcall(0x15, &ireg, &oreg);
	 	ireg.ebx = oreg.ebx; /* Value of ebx is used for further calculation */

	 	/* If CF=1 on the first call means "unsupported function" and on the subsequent calls means "end of list" */
	 	if (oreg.eflags & X86_EFLAGS_CF)
	 		break;

	 	/* After bios interrupt the value of eax == SMAP, if it is not, the 
	 	 * BIOS corrupted our buffer */
	 	if (oreg.eax != SMAP) {
	 		count = 0;
	 		break;
	 	}
	 	*desc++ = buf;
	 	count++;
	} while(oreg.ebx && ARRAY_SIZE(boot_params.e820_map));	/*ebx = 0 implies list is only 1 entry long (worthless) */

	return boot_params.e820_entries = count;
}

int detect_memory(void)
{
	int err = -1;

	if (detect_memory_e820() > 0)
		err = 0;

	return err;
}