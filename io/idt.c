/* The Interrupt Descriptor Table may contain any three types of entries:
 *1) Task gates
 *2) Interrupt gates
 *3) Trap gates
 */

/*

								80386 TASK GATE
   31                23                15                7                0
  +-----------------+-----------------+---+---+---------+-----------------+
  |#############(NOT USED)############| P |DPL|0 0 1 0 1|###(NOT USED)####|4
  |-----------------------------------+---+---+---------+-----------------|
  |             SELECTOR              |#############(NOT USED)############|0
  +-----------------+-----------------+-----------------+-----------------+

                                80386 INTERRUPT GATE
   31                23                15                7                0
  +-----------------+-----------------+---+---+---------+-----+-----------+
  |           OFFSET 31..16           | P |DPL|0 1 1 1 0|0 0 0|(NOT USED) |4
  |-----------------------------------+---+---+---------+-----+-----------|
  |             SELECTOR              |           OFFSET 15..0            |0
  +-----------------+-----------------+-----------------+-----------------+

                                80386 TRAP GATE
   31                23                15                7                0
  +-----------------+-----------------+---+---+---------+-----+-----------+
  |          OFFSET 31..16            | P |DPL|0 1 1 1 1|0 0 0|(NOT USED) |4
  |-----------------------------------+---+---+---------+-----+-----------|
  |             SELECTOR              |           OFFSET 15..0            |0
  +-----------------+-----------------+-----------------+-----------------+

*/

#include <boot.h>
#include <isrs.h>
#include <x86.h>

struct idt_entry {
	unsigned short offset_low;
	unsigned short selector;
	unsigned char zero;
	unsigned char flags;
	unsigned short offset_high;
} __attribute__((packed));

struct idt_ptr {
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

struct idt_entry id[256];
struct idt_ptr idtp;

void set_idt(unsigned char num, unsigned long offset, unsigned short selector, unsigned char flags)
{
	id[num].offset_low = (offset & 0xFFFF);
	id[num].offset_high = (offset >> 16) & 0xFFFF;
	id[num].selector = selector;
	id[num].zero = 0;
	id[num].flags = flags | 0x60; // Present and DPL
}

void idt()
{
	idtp.limit = (sizeof(struct idt_entry)*256)-1;
	idtp.base = (uintptr_t)&id;

	memset(&id, 0, sizeof(struct idt_entry)*256);

  set_idt(0, (unsigned)isr0, 0x08, 0x8E);
  set_idt(1, (unsigned)isr1, 0x08, 0x8E);
  set_idt(2, (unsigned)isr2, 0x08, 0x8E);
  set_idt(3, (unsigned)isr3, 0x08, 0x8E);
  set_idt(4, (unsigned)isr4, 0x08, 0x8E);
  set_idt(5, (unsigned)isr5, 0x08, 0x8E);
  set_idt(6, (unsigned)isr6, 0x08, 0x8E);
  set_idt(7, (unsigned)isr7, 0x08, 0x8E);
  set_idt(8, (unsigned)isr8, 0x08, 0x8E);
  set_idt(9, (unsigned)isr9, 0x08, 0x8E);
  set_idt(10, (unsigned)isr10, 0x08, 0x8E);
  set_idt(11, (unsigned)isr11, 0x08, 0x8E);
  set_idt(12, (unsigned)isr12, 0x08, 0x8E);
  set_idt(13, (unsigned)isr13, 0x08, 0x8E);
  set_idt(14, (unsigned)isr14, 0x08, 0x8E);
  set_idt(15, (unsigned)isr15, 0x08, 0x8E);
  set_idt(16, (unsigned)isr16, 0x08, 0x8E);
  set_idt(17, (unsigned)isr17, 0x08, 0x8E);
  set_idt(18, (unsigned)isr18, 0x08, 0x8E);
  set_idt(19, (unsigned)isr19, 0x08, 0x8E);
  set_idt(20, (unsigned)isr20, 0x08, 0x8E);
  set_idt(21, (unsigned)isr21, 0x08, 0x8E);
  set_idt(22, (unsigned)isr22, 0x08, 0x8E);
  set_idt(23, (unsigned)isr23, 0x08, 0x8E);
  set_idt(24, (unsigned)isr24, 0x08, 0x8E);
  set_idt(25, (unsigned)isr25, 0x08, 0x8E);
  set_idt(26, (unsigned)isr26, 0x08, 0x8E);
  set_idt(27, (unsigned)isr27, 0x08, 0x8E);
  set_idt(28, (unsigned)isr28, 0x08, 0x8E);
  set_idt(29, (unsigned)isr29, 0x08, 0x8E);
  set_idt(30, (unsigned)isr30, 0x08, 0x8E);
  set_idt(31, (unsigned)isr31, 0x08, 0x8E);

	idt_flush((u32)&idtp);
}