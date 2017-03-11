/* Enhanced Disk Drive Information 
 * https://en.wikipedia.org/wiki/INT_13H
 * http://www.ctyme.com/intr/rb-0621.htm
 */


/* These functions are necessary for a BIOS to provide
 * EDD:
 *
 *1) Check Extensions present(41h)
 *2) Get parameters with EDD extensions(48h)
 *3) Set hardware configuration(4Eh)
 *
 */

#include "edd.h"

/* AH = extension number to check extensions 
 * DL = drive number (1st HDD = 80h)
 * BX = 55AAh
 * 
 * CF = set on not present
 * AH = error code or major version number
 * BX = AA55h
 * CX = Interface support bitmask
 */

static int get_edd_info(u8 devno, struct edd_info *ei)
{
	struct biosregs ireg, oreg;

	memcpy(ei, 0, sizeof *ei);

	initregs(&ireg);

	ireg.ah = 0x41;
	ireg.bx = EDDMAGIC1;
	ireg.dl = devno;

	intcall(0x13, &ireg, &oreg);

	if (oreg.eflags & X86_EFLAGS_CF)
		return -1;
	if (oreg.bx != EDDMAGIC2)
		return -1;

	ei->version = oreg.ah; /* EDD version number */

	ei->params.length = sizeof(params);

	/* Extended read drive parameters
	 * AH = 48h = function number for extended_read_drive_parameters 
	 * DL = drive index(1st HDD = 80h)
	 * DS:SI = segment:offset pointer to result buffer 
	 */
	ireg.ah = 0x48;
	ireg.dl = devno;
	ireg.si = (size_t)&ei->params;
	intcall(0x13, &ireg, &oreg);

	/* Read drive parameters
	 * AH = 08h = function number for read_drive_parameters
	 * DL = drive index(1st HDD = 80h)
	 * ES:DI = set to 0000h:0000h to work around some buggy bios
	 */

	ireg.ah = 0x08;
	ireg.dl = devno;
	ireg.es = 0;	/* Ralf Brown recommends setting ES:DI to 0:0 */
	intcall(0x13, &ireg, &oreg);

	if (!(oreg.eflags & X86_EFLAGS_CF))
	{
		ei->physical_number_of_cylinders = oreg.ch + ((oreg.cl & 0xc0) << 2);
		ei->physical_number_of_heads = oreg.dh;
		ei->sectors_per_track = oreg.cl & 0x3f;	/* maximum possible value is 1 to 63 */
	}
	return 0;
}

void query_edd(void)
{
	/*TODO cmdline options */
	struct edd_info ei, *edp;
	u32 mbrptr;
	size_t devno;

	edp = boot_params.eddbuf;
	mbrptr = boot_params.edd_mbr_sig_buf_entries;

	printf("Probing EDD... (edd=off to disable!)\n");

	for (devno = 0x80; devno < 0x80+EDD_MBR_SIG_MAX; devno++)
	{
		if (!get_edd_info(devno, &ei)
			&& boot_params.eddbuf_entries < EDDMAXNR)
		{
			memcpy(edp, &ei, sizeof ei);
			ebp++;
			boot_params.eddbuf_entries++;
		}
		
	}
}