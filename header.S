#include "boot.h"
#include "bootparam.h"

BOOT_SEG = 0x07C0	/* Our boot sector */
SYSSEG	 = 0x1000	/* System gets loaded at 0x10000(65536) */

	.code16
	.section ".bstext" "ax"

	.global bootsect_start

bootsect_start:
	
	ljmp $BOOT_SEG, $start2

start2:
	movw %cs, %ax
	movw %ax, %dx
	movw %ax, %es 
	movw %ax, %ss 
	xorw %sp, %sp  	# Setting stack pointer to zero which on first
	sti				# push will become 0xfffc.

	movw $message, %si

msg_loop:
	lodsb
	andb %al, %al
	jz end
	movw $7, %bx
	movb $0xe, %ah
	int $0x10
	jmp msg_loop

end:
	# Press any key to reboot
	xorw %ax, %ax
	int $0x16
	int $0x19		# Jump to boot sector 0x7C00	  


	.section ".bsdata" "a"

message:
	.ascii	"Use a bootloader.\r\n"
	.ascii	"\n"	
	.ascii	"Press any key to reboot...\r\n"
	.ascii	"\n"
	.byte 	0

# Define the setup headers here which gets copied inside struct		
# This is the first part of kernel setup.

	.section ".header", "a"
	.global	hdr 

hdr:
	setup_sects:	.byte	0
	root_flags:		.word 	1	# If nonzero, root defaults to readonly.
	sys_size:		.long 	0
	ram_size:		.word 	0	# Obsolete	
	vid_mode:		.word 	0
	root_dev:		.word	0
	boot_flag:		.word 	0xAA55

# This is the entry point. Well, not entirely true. It just contains a jump
# instruction to the entry point. This jump instruction is at offset 512.
	.global _start

_start:
	jmp trampoline

# This is the second part of the kernel setup

	.ascii "HdrS"	# Kernel header signature
	.word 	0x0202	# Header version number

realmode_swtch:	.word	0, 0  # A 16-bit far subroutine called just 
							  # before jump to protected mode.
start_sys_seg:	.word	.word SYSSEG 	# A load low seg(0x1000).

				.word kernel_version 	# Pointing to kernel version string

type_of_loader:	.byte 0 	# LILO

loadflags:
		.byte LOADED_HIGH		# Set bit 0

#ifndef __BIG_KERNEL__
	.byte 0
#else
	.byte LOADED_HIGH
#endif

setup_move_size: .word 0x8000	# Size to move if setup is not 
						# loaded at 0x90000. It is started at the 
						# starting of the boot sector.

code32_start:		# The address can be used by the loader to jump
					# to protected mode.

#ifndef __BIG_KERNEL__
	.long 0x1000
#else
	.long 0x100000		# 1MB for big kernel
#endif

ramdisk_image:	.long 0		# A 32-bit linear address for initial ramdisk
					# Zero when no ramfs.

ramdisk_size:	.long 0		# Zero when no ramfs.

bootsect_kludge:	.long 0		# Obsolete

heap_end_ptr:	.word	_end	# We can use address from now on as heap.

ext_loader_ver:	
		.byte 	0
ext_loader_type:
		.byte	0		# Extension to type_of_loader field

cmd_line_ptr:
		.long	0	# If non-zero set this to linear address of 
				# kernel cmdline. The cmdline can lie 
				# anywhere between end of heap/stack and
				# 0xA0000. It doesn't need to be in the
				# 64kb limit.																				

initrd_addr_max:
		.long	0x7fffffff
				# The maximum address that can be occupied
				# by the initial ramdisk.

kernel_alignment:
		.long 0x200000
				# If relocatable kernel is true, this will
				# serve as the alignment required by the 
				# kernel.

relocatable_kernel:
		.byte 1
				# If this byte is non-zero, the protected
				# mode part of the kernel can be loaded at 
				# any address that satsfies the kernel 
				# alignment field. After loading the boot-
				# loader must set the code32_start field 
				# to point to the loaded code.

min_alignment:
		.byte 0		# Minimum alignment

xloadflags:

#ifdef CONFIG_X86_64
# define XLF0	XLF_KERNEL_64	# 64-bit kernel
#else
# define XLF0	0
#endif

#if defined(CONFIG_RELOCATABLE) && (CONFIG_x86_64)
# define XLF1	XLF_CAN_BE_LOADED_ABOVE_4G
#else
# define XLF1	0
#endif														
		.word 	XLF0 | XLF1

cmdline_size:
		.long 2048

hardware_subarch:
		.long 0		# The default execution environment
					# for x86.

hardware_subarch_data:
		.quad 0		# Pointer to specific hardware subarch
					# Unused.

pref_address:
		.quad 0x100000		# Preferred address for loading kernel

init_size:
		.long 0x100000		# Memory for relocating kernel for 
					# safer boot. It is not the size required by 
					# the kernel to boot.

		.section ".entrytext", "ax"

trampoline:
	movw %cs, %ax
	movw %ax, %es
	cld

	# Check if stack segment is set
	movw %ss, %ax
	cmpw %ax, %dx
	movw %sp, %dx
	je 2f

	movw $_end, %dx
	testb $CAN_USE_HEAP, loadflags
	jz 1f
	movw $heap_end_ptr, %dx	

1:
	addw $512, %dx
	jnc 2f

2:
	andw $~3, %dx		# dx points to sp, align it.
	jnz 3f	
	movw $0xfffc, %dx	# Move a 4-byte aligned address.

3:
	# Set correct stack segment
	movw %ax, %ss
	movzl %dx, %esp
	sti

	# Check the boot signature
	cmpl 0x5a5aaa55, setup_sig
	jne setup_bad

	# bss section is in between kernel setup and stack.
	movw $__bss_start, %di 
	movw $_end, %cx 
	xorw %ax, ax
	subw %di, %cx
	shrw $2, %cx 		# divide by sizeof word
	rep; stosl


# Everything here is now done. We will call our main() function
	calll main


	.section ".initdata", "a"

setup_bad:
	movw $setup_corrupt, $si
	calll puts

kernel_version:	.ascii "UTS_RELEASE"
		.ascii	" ("
		.ascii	"LINUX_COMPILE_BY"
		.ascii	"@"
		.ascii 	"LINUX_COMPILE_HOST"
		.ascii	") "
		.ascii	"UTS_VERSION"
		.byte 	0

setup_corrupt:
	.string "No setup signature found... \n"