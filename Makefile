ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif
export E Q

ISODIR	= iso/
PROGRAM	= kernel
ISOPROG	= $(ISODIR)/boot/kernel
OS 		= kernel.iso

OBJS	+= boot.o
#OBJS	+= load.o 
OBJS	+= monitor.o
OBJS	+= io/gdt.o 
OBJS	+= io/idt.o
OBJS	+= io/irq.o
OBJS	+= io/timer.o
OBJS	+= io/isrs.o
OBJS	+= mmu/spinlock.o
OBJS	+= mmu/mmu.o
OBJS	+= mmu/kmalloc.o
OBJS	+= mmu/umalloc.o
OBJS	+= drivers/rtc/clock.c
OBJS 	+= system.o
OBJS	+= kernel.o 

CFLAGS	= -m32 -Iinclude -ffreestanding -std=gnu99 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -g

$(PROGRAM):	$(OBJS)
	$(E) "  LINK	  " $@
	$(Q) $(CC) -Wl,--build-id=none $(OBJS) $(CFLAGS) -T kernel.ld -o $@
	$(E) "  CP	  " $@
	$(Q) cp $(PROGRAM) iso/boot
	$(E) "  ISO 	  " $@
	$(Q) grub-mkrescue -o '$@'.iso '$(ISODIR)'

$(OBJS):
%.o: %.c 
	$(E) "  CC      " $@
	$(Q) $(CC) -c $(CFLAGS) $< -o $@	

boot.o: boot.S
	$(E) "  CC      " $@
	$(Q) $(CC) -m32 -Iinclude -gdwarf-2 -Wa,-divide -c boot.S -o boot.o

#load.o: load.S
#	$(E) "  CC      " $@
#	$(Q) $(CC) -m32 -Iinclude -gdwarf-2 -Wa,-divide -c load.S -o load.o	


bochs: $(PROGRAM)
		CYLINDERS="$$(($$(stat -c '%s' '$(PROGRAM)') / 512))" && \
		bochs -qf /dev/null \
			'ata0-master: type=disk, path="kernel.iso", mode=flat, cylinders='"$$CYLINDERS"', heads=1, spt=1' \
			'boot: disk' \
			'display_library: sdl, options="gui_debug"' \
			'megs: 128'	

clean:
	$(E) "  CLEAN"
	$(Q) rm -f *.o 
	$(Q) rm -f io/*.o 
	$(Q) rm -f mmu/*.o
	$(Q) rm -f kernel
	$(Q) rm -f kernel.iso
	$(Q) rm -f iso/boot/kernel
.PHONY: clean
