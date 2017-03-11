#ifndef _ASM_X86_LINKAGE_H
#define _ASM_X86_LINKAGE_H

#ifndef __ASSEMBLY__
#define GLOBAL(name)	\
	.globl name;		\
	name:

#if defined(CONFIG_X86_64) || (CONFIG_X86_ALIGN_16)
#define __ALIGN		.p2align 4, 0x90
#define __ALIGN_STR		__stringify(__ALIGN)
#endif

#endif /* __ASSEMBLY__ */
#endif /* _ASM_X86_LINKAGE_H */	