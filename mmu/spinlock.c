/* Mutual exclusion spinlock */

#include <boot.h>
#include <mmu.h>
#include <proc.h>

/* Acquires lock by spinning until lock is acquired.
 * We first disable the interrupts so that the deadlock condition 
 * is avoided.
 */
void acquire(struct spinlock *lk)
{
	pushcli();

	if (holding(lk))
		kprintf("acquire\n");

	/* Atomic xchg instruction for locking */
	while(xchg(lk->locked, 1) != 0)
		;

	/* 
	 * GCC built-in instruction which acts as a memory barrier and
	 * ensures all prior read or write to complete before subsequent
	 * read or writes are executed using the "MFENCE" instruction.
	 * This also prevents the reordering of the loads or stores, which
	 * can be done by the compiler or the hardware.
	 */
	__sync_synchronize();
}

void release(struct spinlock *lk)
{
	if (!holding(lk))
		kprintf("release");

	__sync_synchronize();

	asm volatile("movl $0, %0" : "+m" (lk->locked) : );

	popcli();
}

/* Check weather the cpu is holding the lock */
int holding(struct spinlock *lk)
{
	return lk->locked && lk->cpu == cpu;
}

void pushcli(void)
{
	int eflags;

	eflags = read_flags();
	cli();
	if (cpu->pushcli == 0)
	{
		cpu->intr = eflags & IF;
	}
	cpu->pushcli +=1;
}

void popcli(void)
{
	if (read_flags() & IF)
		kprintf("popcli - interruptable");
	if ((cpu->pushcli && cpu->intr) == 0)
		sti();
}