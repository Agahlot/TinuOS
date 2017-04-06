/* Memory Management defines */

struct spinlock {
	u32 locked;	/* is the lock head, 0=Available;non-zero=held */

	/* For debugging puposes */
	char *name;		/* name of the lock */
	struct cpu *cpu;	/* Is it the same CPU that released the lock who was holding the lock */
	u32 pcs[10];		/* The call stack that locked the lock */
};


/* EFLAGS */
#define CF			0x00000001		/* Carry Flag */
#define PF			0x00000004 		/* Parity Flag */
#define AF			0x00000010 		/* Auxiliary Flag */
#define ZF			0x00000040 		/* Zero Flag */
#define SF			0x00000800 		/* Sign Flag */
#define TF			0x00000100 		/* Trap Flag */
#define IF			0x00000200 		/* Interrupt Enable Flag */
#define DF 			0x00000400 		/* Direction Flag */
#define OF 			0x00000800 		/* Overflow Flag */
#define IOPL_MASK	0x00003000 		/* I/O Privilege Level mask */
#define IOPL_0		0x00000000 		/* I/O Level 0 */
#define IOPL_1		0x00001000 		/* I/O Level 1 */
#define IOPL_2		0x00002000 		/* I/O Level 2 */
#define IOPL_3		0x00003000 		/* I/O Level 3 */
#define NT			0x00004000 		/* Nested Task */
#define RF			0x00010000 		/* Resume Flag */
#define VM			0x00020000 		/* Virtual-8086 Mode */
#define AC			0x00040000 		/* Alignment Check */
#define VIF			0x00080000 		/* Virtual Interrupt Flag */
#define VIP			0x00100000 		/* Virtual Interrupt Pending */
#define ID			0x00200000 		/* ID Flag */

#define PAGE_CACHEABLE_SHIFT		0
#define PAGE_NOT_CACHEABLE_SHIFT 	PAGE_CACHEABLE_SHIFT
#define PAGE_PRESENT_SHIFT			1
#define PAGE_NOT_PRESENT_SHIFT		PAGE_PRESENT_SHIFT
#define PAGE_USER_SHIFT				2
#define PAGE_KERNEL_SHIFT			PAGE_USER_SHIFT
#define PAGE_READ_SHIFT				3
#define PAGE_WRITE_SHIFT			4
#define PAGE_EXEC_SHIFT				5
#define PAGE_GLOBAL_SHIFT			6

#define PAGE_NOT_CACHEABLE 			(0 << PAGE_NOT_CACHEABLE_SHIFT)
#define PAGE_CACHEABLE 				(1 << PAGE_CACHEABLE_SHIFT)

#define PAGE_PRESENT 				(0 << PAGE_PRESENT_SHIFT)
#define PAGE_NOT_PRESENT 			(1 << PAGE_PRESENT_SHIFT)

#define PAGE_USER 					(1 << PAGE_USER_SHIFT)
#define PAGE_KERNEL 				(0 << PAGE_KERNEL_SHIFT)

#define PAGE_READ 					(1 << PAGE_READ_SHIFT)
#define PAGE_WRITE 					(1 << PAGE_WRITE_SHIFT)
#define PAGE_EXEC 					(1 << PAGE_EXEC_SHIFT)

#define PAGE_GLOBAL 				(1 << PAGE_GLOBAL_SHIFT)

/* spinlock.c */
void 	pushcli(void);
int 	holding(struct spinlock*);