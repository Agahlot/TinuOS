#include "boot.h"
#include "string.h"

#define CONFIG_X86_MINIMUM_CPU_FAMILY	64

static const int req_level = CONFIG_X86_MINIMUM_CPU_FAMILY;


#define cpuid(in, a, b, c, d) asm("cpuid" :"=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));										
						
int do_intel(int *cpu_level_ptr, int *req_level_ptr)
{										
	unsigned long ebx, unused;
	cpuid(0, unused, ebx, unused, unused);
	switch(ebx)
	{
		case 0x756e6547:	/* Intel magic code */
			unsigned long eax, ebx, ecx, edx, max_eax, signature, unused;
			int model, family, type;
			cpuid(1, eax, ebx, unused, unused);	//	31			13  12 11	   8 7		 3 4	  0
			model = (eax >> 4) & 0xf;			//	-----------------------------------------------
			family = (eax >> 8) & 0xf;			//	|			| TYPE|  FAMILY	|  MODEL  | Steppi|
			type = (eax >> 12) & 0x3;			//	|			|	  |			|		  |	  ID  |
												//	----------------------------------------------- 
			
			if (family == 6 && (model == 9 || model == 13))
			{
				puts("PAE is diabled at boot. You can enable by using 'forcepae'!\n");
			}

			*cpu_level_ptr = family;
			*req_level_ptr = req_level;

			return (cpu_level_ptr < req_level_ptr) ? -1 : 0;

			break;
		default:
			puts("Unknown x86 Processor detected\n");
			break
	}
	return 0;
}