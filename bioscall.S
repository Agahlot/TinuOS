	.code16
	.section ".inttext", "ax"
	.globl intcall
	.type intcall, @function

intcall:
	cmpb %al, 3f
	je 1f
	movb %al, 3f
	jmp 1f

1:
	/* save register state */
	pushfl
	pushw %gs
	pushw %fs
	pushal

	/* copy iregs to stack */
	subw $44, %sp
	movw %sp, %di
	movw %dx, %si 
	movb $11, %cx	/* There are 11 32-bit registers defined boot.h */
	rep; movsd

	/* pop full state from stack */
	popal
	popw %fs
	popw %gs
	popw %es
	popw %ds
	popfl

	.byte 0xcd	/* INT opcode */
3:	.byte 0

	/* push full state to stack */
	pushfl
	pushw %ds
	pushw %es
	pushw %gs
	pushw %fs
	pushal

	/* es and ds registers are disturbed, so set them */
	cld 
	movzwl %sp, %esp
	movw %cs, %ax
	movw %ax, %es
	movw %ax, %ds 

	/* Now we have to copy the output state from stack to oregs */
	movw 68(%esp), %di
	andw %di, %di
	jz 4f
	movw %sp, %si
	rep; movsd

4:
	addw $44, %sp

	/* restore state and return */
	popal
	popw %fs
	popw %gs
	popfl
	retl	

	.size	intcall, .-intcall