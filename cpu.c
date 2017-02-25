#include "boot.h"
#include "string.h"

static char *cpu_name(int level)
{
	static char buf[6];

	if (level == 64)
		return "x86-64";
	else {
		if (level == 15)
			level = 6;
		sprintf(buf, "i%d86", level);
		return buf;
	}
}

int validate_cpu()
{
	int cpu_level, req_level;
	do_intel(&cpu_level, &req_level);

	if (cpu_level < req_level)
	{
		printf("This kernel requires an %s CPU, ",
			cpu_name(req_level));
		printf("but only detected an %s CPU.\n",
			cpu_name(cpu_level));
		return -1;
	}
}