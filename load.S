# We can boot the TinuOS in either of the two ways:
# 1) Mapping from VA[0 - 4MB] to PA[0 - 4MB] and also to the higher address (defined in boot.S)
# 2) Mappping from VA[0 - 4MB] to PA[0 - 4MB] and to higher address and then invalidating the
#    lower memory so that the first 4MB of the physical memory is mapped to KERNBASE and we 
#    would have a higher half kernel. (defined in load.S)

#include <memlayout.h>
#include <mmu.h>

.set ALIGN, 	1<<0
.set MEMINFO,	1<<1
.set FLAGS,		ALIGN | MEMINFO
.set MAGIC,		0x1BADB002
.set CHECKSUM,	-(MAGIC + FLAGS)

# The Multiboot header should be defined in a section and also reserve a section
# in kernel linker script otherwise and error related to Multiboot header not found
# will be shown.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .text
# Entry Point
.globl _start
_start:

# The multiboot standard does not define the value of the stack pointer register
# (esp) and it is up to the kernel to provide a stack. The stack gets allocated 
# in data section and according to the kernel linking, it is located at a high 
# address. The stack grows downwards on x86. The stack on x86 must be 16-byte aligned 
# according to the System V ABI standard and de-facto extensions. The compiler will
# assume the stack is properly aligned and failure to align the stack will result in
# undefined behavior.
	
	# Use 4MB pages
	movl    %cr4, %eax
  	orl     $(CR4_PSE), %eax
  	movl    %eax, %cr4
  	# Physical address of Page Directory
  	movl    $(V2P_WO(entrypgdir)), %eax
  	movl    %eax, %cr3
  	# Turn on paging
  	movl    %cr0, %eax
  	orl     $(CR0_PG|CR0_WP), %eax
  	movl    %eax, %cr0

  	movl $start_high, %ecx
  	jmp *%ecx

start_high:

	movl $(V2P_WO(entrypgdir)), %ecx
	movl $0, (%ecx) 
	invlpg 0x0 	

	movl $(stack + 4096), %esp

	pushl %eax                           
 
    pushl %ebx

	movl $kernel_main, %eax
	jmp *%eax

.macro ISR_NOERRORCODE code
.global isr\code
isr\code:
	cli
	pushl $0
	pushl $\code
	jmp isr_common_stub
.endm

.macro ISR_ERRORCODE code
.global isr\code
isr\code:
	cli
	pushl $\code
	jmp isr_common_stub
.endm	

.macro IRQ_ERRORCODE one, two
.global irq\one
irq\one:
	cli
	push $0
	pushl $\two
	jmp irq_common_stub
.endm		

ISR_NOERRORCODE 0
ISR_NOERRORCODE 1
ISR_NOERRORCODE 2
ISR_NOERRORCODE 3
ISR_NOERRORCODE 4
ISR_NOERRORCODE 5
ISR_NOERRORCODE 6
ISR_NOERRORCODE 7
ISR_ERRORCODE   8
ISR_NOERRORCODE 9
ISR_ERRORCODE   10
ISR_ERRORCODE   11
ISR_ERRORCODE   12
ISR_ERRORCODE   13
ISR_ERRORCODE   14
ISR_NOERRORCODE 15
ISR_NOERRORCODE 16
ISR_ERRORCODE 17
ISR_ERRORCODE 18
ISR_ERRORCODE 19
ISR_ERRORCODE 20
ISR_ERRORCODE 21
ISR_ERRORCODE 22
ISR_ERRORCODE 23
ISR_ERRORCODE 24
ISR_ERRORCODE 25
ISR_ERRORCODE 26
ISR_ERRORCODE 27
ISR_ERRORCODE 28
ISR_ERRORCODE 29
ISR_ERRORCODE 30
ISR_ERRORCODE 31

IRQ_ERRORCODE 0, 32
IRQ_ERRORCODE 1, 33
IRQ_ERRORCODE 2, 34
IRQ_ERRORCODE 3, 35
IRQ_ERRORCODE 4, 36
IRQ_ERRORCODE 5, 37
IRQ_ERRORCODE 6, 38
IRQ_ERRORCODE 7, 39
IRQ_ERRORCODE 8, 40
IRQ_ERRORCODE 9, 41
IRQ_ERRORCODE 10, 42
IRQ_ERRORCODE 11, 43
IRQ_ERRORCODE 12, 44
IRQ_ERRORCODE 13, 45
IRQ_ERRORCODE 14, 46
IRQ_ERRORCODE 15, 47

.extern isr_handler
isr_common_stub:
	pushal
	movw %ds, %ax
	pushl %eax
	movw $0x10, %ax		# Load kernel data segment descriptor
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	call isr_handler
	popl %ebx			# Reload the original data segment descriptor
	movw %bx, %ds
	movw %bx, %es
	movw %bx, %fs
	movw %bx, %gs
	popal
	addl $8, %esp
	sti
	iretl

.extern irq_handler
irq_common_stub:
	pushal
	movw %ds, %ax
	pushl %eax
	movw $0x10, %ax		# Load kernel data segment descriptor
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	call irq_handler
	popl %ebx			# Reload the original data segment descriptor
	movw %bx, %ds
	movw %bx, %es
	movw %bx, %fs
	movw %bx, %gs
	popal
	addl $8, %esp
	sti
	iretl
	
.global gdt_flush
.extern gdpt
.type gdt_flush, @function
gdt_flush:
	movl 4(%esp), %eax
	lgdt (%eax)
	# Our code descriptor is 8 bytes offset from start of gdt
	movw $0x10, %ax
	movw %ax, %ds 
	movw %ax, %es 
	movw %ax, %fs 
	movw %ax, %gs 
	movw %ax, %ss 
	ljmp $0x08,$flush2
flush2:
	ret

.global tss_flush
.type tss_flush, @function
tss_flush:
	movw $0x10, %ax
	ltr %ax
	ret

.global idt_flush
.extern idtp
.type idt_flush, @function
idt_flush:
	movl 4(%esp), %eax
	lidt (%eax)
	ret

.global die 
.type die, @function
die:
	hlt 
	jmp die

.comm stack, 4096, 16