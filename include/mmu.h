/* Memory Management defines */

struct spinlock {
	u32 locked;	/* is the lock head, 0=Available;non-zero=held */

	/* For debugging puposes */
	char *name;		/* name of the lock */
	struct cpu *cpu;	/* CPU holding the lock */
	u32 pcs[10];		/* The call stack that locked the lock */
};


/* spinlock.c */
void 	pushcli(void);
int 	holding(struct spinlock*);