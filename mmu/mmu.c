#include <boot.h>
#include <mmu.h>
#include <paging.h>
#include <memlayout.h>

extern char *data[];
u32 *kernpgdir;

u32 *pagewalk(u32 *pgdir, void *va, int alloc)
{
	u32 *pde;
	u32 *pte;

	pde = &pgdir[PDE(va)];
	/* If not present then, Page Table hasn't been allocated */
	if (*pde & PTE_P) {
		pte = P2V(*pde);		
	}
	/* Allocate a Page Table and put its Physical Address in Page Directory */
	else {
		pte = (u32 *)kalloc();
		memset(pte, 0, PAGE_SIZE);
		*pde = (V2P(pte)) | PAGE_PRESENT | PAGE_READ | PAGE_WRITE | PAGE_USER;
	}
	return &pte[PTE(va)];
}
	
u32 mappages(u32 *pgdir, void *va, u32 size, u32 pa, int perm)
{
	char *start, *end;
	u32 *pte;

	start = (char *)va;
	end = (char *)(va + size - 1);
	for(;;)
	{
		if (*pte = (pagewalk(pgdir, start, 1)) == 0)
			return -1;

		/* If the page is already mapped */
		if (*pte & PTE_P) {
			kprintf("remapped");
			return 0;
		}
		/* Page Table containing the Physical Address Frame */
		*pte = pa | perm | PTE_P;

		if (start == end)
			break;
		start += PAGE_SIZE;
		end += PAGE_SIZE;
	}

	return 0;
}

/* Memory mapping */
static struct kmap {
	void *virt;
	u32 phy_start;
	u32 phy_end;
	int perm;
} kmap [] = {
	{(void *)KERNBASE,	0,				EXTMEM,		PTE_RW },
	{(void *)KERNLINK,	V2P(KERNLINK),	V2P(data),	0	   },
	{(void *)data,		V2P(data),		PHYSTOP,	PTE_RW },
	{(void *)DEVSPACE,	DEVSPACE,		0,			PTE_RW },
};

/* Allocate a page of memory to hold the Page Directory.
 * Then, Virtual to Physical memory translations are installed.
 * The translations for user memory are defined later.
 */
u32 *pagetable_init(void)
{
	/* Allocate a 4kb block for kernel Page Directory and nullify it */
	u32 *pgdir;
	struct kmap *k;

	if ((pgdir = (u32 *)kalloc()) == 0)
		return 0;
	memset(pgdir, 0, PAGE_SIZE);

	/* Install the memory mapping */
	for(k = kmap; k < &kmap[ARRAY_SIZE(kmap)]; k++)
	{
		if (mappages(pgdir, k->virt, k->phy_end - k->phy_start, k->phy_start, k->perm) < 0)
			return 0;
	}
	return pgdir;
}

void paging_install(void)
{
	kernpgdir = pagetable_init();
}

void page_fault(void)
{
	u32 faulting_address;
	asm volatile("movl %%cr2, %0" : "=r" (faulting_address));

	u32 present = !(regs.err_code & 0x1);
	u32 rw = regs.err_code & 0x2;
	u32 us = regs.err_code & 0x8;
	u32 id = regs.err_code & 0x10;

	kprintf("Page Fault! ( ");
	if (present) {kprintf("present ");}
	if (rw) {kprintf("read-only ");}
	if (us) {kprintf("user-mode ");}
	kprintf(") at 0x");
	terminal_write_hex(faulting_address);
	kprintf("\n");
	die();
}