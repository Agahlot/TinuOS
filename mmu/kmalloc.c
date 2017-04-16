#include <boot.h>
#include <spinlock.h>
#include <paging.h>
#include <memlayout.h>

extern char end[];

struct run {
	struct run *next;
};

struct {
	struct spinlock lock;
	int use_lock;
	struct run *freelist;
} kmem;

void kinit1(void *vstart, void *vend)
{
	initlock(&kmem.lock, "kmem");
	kmem.use_lock = 0;
	freerange(vstart, vend);
}

void freerange(void *vstart, void *vend)
{
	char *p;
	p = (char *)(PGROUNDUP((u32)vstart));
	for(; p+PAGE_SIZE <= (char *)vend; p+=PAGE_SIZE)
		kfree(p);
}

void kfree(char *p)
{
	struct run *r;

	if ((u32)p >= V2P(PHYSTOP))
		kprintf("kfree");

	memset(p, 1, PAGE_SIZE);

	if (kmem.use_lock)
		acquire(&kmem.lock);
	r = (struct run *)p;
	r->next = kmem.freelist;
	kmem.freelist = r;
	if (kmem.use_lock)
		release(&kmem.lock);
}

/* Allocates a PAGE_SIZE memory */
char *kalloc()
{
	struct run *r;

	if (kmem.use_lock)
		acquire(&kmem.lock);
	r = kmem.freelist;
	if (r)
		kmem.freelist = r->next;	/* Points to next free page */
	if (kmem.use_lock)
		release(&kmem.lock);

	return (char *)r;
}