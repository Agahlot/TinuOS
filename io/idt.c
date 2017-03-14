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
#include <irq.h>
#include <x86.h>

extern isr_t interrupt_descriptors[];

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

typedef struct idt_entry idt_entry_t;
typedef struct idt_ptr idt_ptr_t;

idt_entry_t id[256];
idt_ptr_t idtp;

void set_idt(u32 num, u32 offset, u16 selector, u8 flags)
{
	id[num].offset_low = (offset & 0xFFFF);
	id[num].offset_high = (offset >> 16) & 0xFFFF;
	id[num].selector = selector;
	id[num].zero = 0;
	id[num].flags = flags | 0x60; // Present and DPL
}

void idt()
{
	idtp.limit = (sizeof(idt_entry_t)*256)-1;
	idtp.base = (uintptr_t)&id;

	memset(&id, 0, sizeof(idt_entry_t)*256);

  irq_remap();

  set_idt(0, (u32)  isr0, 0x08, 0x8E);
  set_idt(1, (u32)  isr1, 0x08, 0x8E);
  set_idt(2, (u32)  isr2, 0x08, 0x8E);
  set_idt(3, (u32)  isr3, 0x08, 0x8E);
  set_idt(4, (u32)  isr4, 0x08, 0x8E);
  set_idt(5, (u32)  isr5, 0x08, 0x8E);
  set_idt(6, (u32)  isr6, 0x08, 0x8E);
  set_idt(7, (u32)  isr7, 0x08, 0x8E);
  set_idt(8, (u32)  isr8, 0x08, 0x8E);
  set_idt(9, (u32)  isr9, 0x08, 0x8E);
  set_idt(10, (u32)  isr10, 0x08, 0x8E);
  set_idt(11, (u32)  isr11, 0x08, 0x8E);
  set_idt(12, (u32)  isr12, 0x08, 0x8E);
  set_idt(13, (u32)  isr13, 0x08, 0x8E);
  set_idt(14, (u32)  isr14, 0x08, 0x8E);
  set_idt(15, (u32)  isr15, 0x08, 0x8E);
  set_idt(16, (u32)  isr16, 0x08, 0x8E);
  set_idt(17, (u32)  isr17, 0x08, 0x8E);
  set_idt(18, (u32)  isr18, 0x08, 0x8E);
  set_idt(19, (u32)  isr19, 0x08, 0x8E);
  set_idt(20, (u32)  isr20, 0x08, 0x8E);
  set_idt(21, (u32)  isr21, 0x08, 0x8E);
  set_idt(22, (u32)  isr22, 0x08, 0x8E);
  set_idt(23, (u32)  isr23, 0x08, 0x8E);
  set_idt(24, (u32)  isr24, 0x08, 0x8E);
  set_idt(25, (u32)  isr25, 0x08, 0x8E);
  set_idt(26, (u32)  isr26, 0x08, 0x8E);
  set_idt(27, (u32)  isr27, 0x08, 0x8E);
  set_idt(28, (u32)  isr28, 0x08, 0x8E);
  set_idt(29, (u32)  isr29, 0x08, 0x8E);
  set_idt(30, (u32)  isr30, 0x08, 0x8E);
  set_idt(31, (u32)  isr31, 0x08, 0x8E);

  set_irq();

	idt_flush((u32)&idtp);

  /* NULLify all the interrupt descriptors */
  memset(&interrupt_descriptors, 0, sizeof(isr_t)*256);
}