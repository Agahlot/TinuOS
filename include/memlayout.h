
#define EXTMEM		0x100000	/* 4MB */
#define PHYSTOP		0xE000000	/* Top physical memory */
#define DEVSPACE	0xFE000000	/* Devices mapped at high addresses */

#define KERNBASE	0x80000000	/* Kernel Virtual Address (2GB) */
#define KERNLINK	(KERNBASE+EXTMEM)	/* Address where kernel is linked */

#define V2P(a)	(((u32) (a)) - KERNBASE)
#define P2V(a)	(((void *) (a)) + KERNBASE)

#define V2P_WO(a)	((a) - KERNBASE)
#define P2V_WO(a)	((a) + KERNBASE)