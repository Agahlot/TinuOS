/* Mutual exclusion spinlock */

#include <boot.h>
#include <spinlock.h>

/* Check weather the cpu is holding the lock */
int holding(lock *lk)
{
	return lk->locked;
}

void pushcli(void)
{
	int eflags;

	eflags = read_eflags();
	cli();
/*	if (cpu->pushcli == 0)
	{
		cpu->intr = eflags & IF;
	}
	cpu->pushcli +=1;
*/
}

void popcli(void)
{
	if (read_eflags() & IF)
		kprintf("popcli - interruptable");
//	if ((cpu->pushcli && cpu->intr) == 0)
		sti();
}

void initlock(lock *lk, char *name)
{
	lk->name = name;
	lk->locked = 0;
//	lk->cpu = 0;
}

/* Acquires lock by spinning until lock is acquired.
 * We first disable the interrupts so that the deadlock condition 
 * is avoided.
 */
void acquire(lock *lk)
{
	pushcli();

	if (holding(lk))
		kprintf("acquire\n");

	/* Atomic xchg instruction for locking */
	while(xchg(&lk->locked, 1) != 0)
		;

	/* 
	 * GCC built-in instruction which acts as a memory barrier and
	 * ensures all prior read or write to complete before subsequent
	 * read or writes are executed using the "MFENCE" instruction.
	 * This also prevents the reordering of the loads or stores, which
	 * can be done by the compiler or the hardware.
	 */
	__sync_synchronize();

//	lk->cpu = cpu;
}

void release(lock *lk)
{
	if (!holding(lk))
		kprintf("release");

	__sync_synchronize();

//	lk->cpu = 0;

	asm volatile("movl $0, %0" : "+m" (lk->locked) : );

	popcli();
}
