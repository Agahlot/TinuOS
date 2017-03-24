struct cpu {
	u8 acpid;
	volatile u32 started;	/* Has the cpu started */
	int pushcli;			/* Depth of pushcli nesting */
	int intr;				/* Were interrupts enabled before pushcli */
};