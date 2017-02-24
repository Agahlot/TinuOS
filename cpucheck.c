#include "boot.h"
#include "string.h"

int do_intel(void);
int cpucheck(void);

#define cpuid(in, a, b, c, d) asm("cpuid" :"=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

int cpucheck()
{
	unsigned long eax, ebx, ecx, edx, max_eax, signature, unused;
	int model, family, type;
	cpuid(1, eax, ebx, unused, unused);
	model = (eax >> 4) & 0xf;
	family = (eax >> 8) & 0xf;
	type = (eax >> 12) & 0x3;
}

int do_intel()
{
	unsigned long ebx, unused;
	cpuid(0, unused, ebx, unused, unused);
	switch(ebx)
	{
		case 0x756e6547:	/* Intel magic code */
			cpucheck();
			break;
		default:
			puts("Unknown x86 Processor detected\n");
			break
	}
	return 0;
}