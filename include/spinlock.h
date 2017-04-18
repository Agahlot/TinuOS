#pragma once

struct spinlock {
	u32 locked;	/* is the lock head, 0=Available;non-zero=held */

	/* For debugging puposes */
	char *name;		/* name of the lock */
	struct cpu *cpu;	/* Is it the same CPU that released the lock who was holding the lock */
	u32 pcs[10];		/* The call stack that locked the lock */
};

typedef struct spinlock lock;
lock *lk;