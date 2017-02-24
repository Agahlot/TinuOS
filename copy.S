#include <linkage.h>

	.code16
	.text

GLOBAL(memcpy)
	pushw %si
	pushw %di
	movw %ax, %di	
	movw %dx, %si
	pushw %cx
	shrw $2, %cx
	rep; movsl		# move from %si to %es:%di
	popw %cx
	andw $3, %cx	# align by 4 bytes
	rep; movsb
	popw %di
	popw %si
	retl
ENDPROC(memcpy)	

GLOBAL(memset)
	pushw %di
	movw %ax, %di
	movzbl %dl, %eax
	imull $0x01010101, %eax
	pushw %cx
	shrw $2, %cx
	rep; stosl
	popw %cx
	andw $3, %cx
	rep; stosb
	popw %di
	retl
ENDPROC(memset)	