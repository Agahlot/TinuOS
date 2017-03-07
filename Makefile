ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif
export E Q

PROGRAM	= kernel

OBJS	+= boot.o 
OBJS	+= copy.o
OBJS	+= io/gdt.o 
OBJS	+= io/idt.o
OBJS	+= kernel.o 

CFLAGS	= -m32 -Iinclude -std=gnu99 -ffreestanding -O2

$(PROGRAM):	$(OBJS)
	$(E) "	LINK	" $@
	$(Q) $(CC) $(OBJS) -m32 -T kernel.ld -ffreestanding -O2 -nostdlib -o $@ -lgcc

$(OBJS):
%.o: %.c 
	$(E) "  CC      " $@
	$(Q) $(CC) -c $(CFLAGS) $< -o $@	

boot.o: boot.s 
	$(E) "  CC      " $@
	$(Q) $(CC) -m32 -c boot.s -o boot.o

copy.o: copy.S
	$(E) "  CC      " $@
	$(Q) $(CC) -m32 -c -Iinclude copy.S -o copy.o

clean:
	$(E) "  CLEAN"
	$(Q) rm -f *.o 
	$(Q) rm -f io/*.o 
.PHONY: clean