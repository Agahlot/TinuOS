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

void set_idt(int num, u32 offset, u16 selector, u8 flags)
{
	id[num].offset_low = (offset & 0xFFFF);
	id[num].offset_high = (offset >> 16) & 0xFFFF;
	id[num].selector = (selector);
	id[num].zero = 0;
	id[num].flags = flags | 0x60;	// Present and DPL;
}

void idt()
{
	idtp.limit = (sizeof(struct idt_entry)*256)-1;
	idtp.base = (unsigned int)&id;

	memset(&id, 0, (sizeof(struct idt_entry)*256));

	load_idt();
}