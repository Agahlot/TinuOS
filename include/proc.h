#pragma once
#include <x86.h>

struct cpu {
	u8 acpid;
	volatile u32 started;	/* Has the cpu started */
	int pushcli;			/* Depth of pushcli nesting */
	int intr;				/* Were interrupts enabled before pushcli */

	struct cpu *cpu;
};

extern struct cpu cpus[8];