

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

/* Control Registers */
#define CR0_PE 		0x00000001 		/* Protected Mode Enable */
#define CR0_MP		0x00000002 		/* Monitor co-processor */
#define CR0_WP		0x00010000 		/* Write Protect */
#define CR0_CD		0x40000000 		/* Cache Disable */
#define CR0_PG 		0x80000000		/* Paging */

#define CR4_PSE		0x00000010 		/* Page Size Extension */
#define CR4_PAE		0x00000020 		/* Physical Address Extension */

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
