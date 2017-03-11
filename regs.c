/* Initializing basic register sets */

#include "boot.h"
#include "string.h"

void initregs(struct biosregs *regs)
{
	memset(regs, 0, sizeof *regs);
	regs->ds = ds();
	regs->es = ds();
	regs->fs = fs();
	regs->gs = gs();
}