ifeq ($(strip $(V)),)
	E = @echo
	Q = @
else
	E = @\#
	Q =
endif
export E Q

PROGRAM	= main

OBJS	+= copy.o
OBJS	+= header.o
OBJS	+= early_serial_printk.o
OBJS	+= printk.o
OBJS	+= main.o 

uname_M      := $(shell uname -m | sed -e s/i.86/i386/)
ifeq ($(uname_M),i386)
	DEFINES      += -DCONFIG_X86_32
ifeq ($(uname_M),x86_64)
	DEFINES      += -DCONFIG_X86_64
endif
endif

CC = gcc
CFLAGS = -Wall -Werror -pedantic -m32 -O0 -finline-functions -fno-stack-protector -nostdinc -ffreestanding -Wno-unused-function -Wno-unused-parameter -g
LD = ld -m elf_i386

all: $(PROGRAM)

$(OBJS):

%.o: %.c
	$(E) "  CC      " $@
	$(Q) $(CC) -c $(CFLAGS) $< -o $@
	$(E) "  LINK    " $@
	$(Q) $(LD) -T setup.ld -o main $<

.PHONY: clean

clean:
	$(E) "  CLEAN"
	$(Q) rm -f *.o	 
