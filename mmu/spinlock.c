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

	/* acquire lock by atomic instruction */
	if (holding(lk))
}

/* Check weather the cpu is holding the lock */
int holding(struct spinlock *lk)
{
	return lk->locked 
}