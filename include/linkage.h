#ifndef __LINUX_LINKAGE_H
#define __LINUX_LINKAGE_H

#include <asm/linkage.h>
#include <linux/stringify.h>


#ifndef __ALIGN
#define __ALIGN		.align 4, 0x90	/* Next address that is evenly divisible by 4 */
#define __ALIGN_STR	".align 4, 0x90"
#endif 

#ifndef ENTRY
#define ENTRY(name)	\
	.globl name		\
	name:
#endif	

#ifndef WEAK
#define WEAK(name)	\
	.globl name		\
	name:
#endif

#ifndef END 		
#define END(name)	\
	.size name, .-name
#endif

#ifndef ENDPROC
#define ENDPROC(name)	\
	.type name, @function	\
	END(name)			\
#endif

#endif /* __LINUX_LINKAGE_H */