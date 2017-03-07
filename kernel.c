/* Surely you will remove the processor conditionals and this comment
   appropriately depending on whether or not you use C++. */
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
//#include <stdio.h>
//#include <stddef.h>
//#include <stdint.h>
#include <multiboot.h>
#include <x86.h>
#include <boot.h>
#define cpuid(in, a, b, c, d) __asm__("cpuid": "=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (in));

void printregs(int eax, int ebx, int ecx, int edx);
/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline u8 vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline u16 vga_entry(unsigned char uc, u8 color) {
	return (u16) uc | (u16) color << 8;
}
 
size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
u8 terminal_color;
u16* terminal_buffer;
 
void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (u16*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(u8 color) {
	terminal_color = color;
}

static void
itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  
  /* If %d is specified and D is minus, put `-' in the head.  */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;

  /* Divide UD by DIVISOR until UD == 0.  */
  do
    {
      int remainder = ud % divisor;
      
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);

  /* Terminate BUF.  */
  *p = 0;
  
  /* Reverse BUF.  */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}
 
void terminal_putentryat(char c, u8 color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}
 
void terminal_putchar(char c) {
	if (c == '\n')
	{
		terminal_column = 0;
		terminal_row++;
	}
	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row = 0;
	}
}
 
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void kprintf(const char* data, ...) {
  char **arg = (char **) &data;
  int c;
  char buf[20];

  arg++;
  
  while ((c = *data++) != 0)
    {
      if (c != '%')
	terminal_putchar (c);
      else
	{
	  char *p;
	  
	  c = *data++;
	  switch (c)
	    {
	    	case 'd':
	    	case 'u':
	    	case 'x':
	    		itoa (buf, c, *((int *) arg++));
	      	  	p = buf;
	     	  	goto string;
	      	  	break;

	    	case 's':
	      		p = *arg++;
	      		if (! p)
					p = "(null)";

	    	string:
	     		 while (*p)
					terminal_putchar (*p++);
	     		 break;

	    	default:
	      		terminal_putchar (*((int *) arg++));
	     	 break;
	    }
	}
    }
}

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(unsigned long magic, unsigned long addr)
{
	unsigned long eax,ebx,ecx,edx,max_eax,unused;
	terminal_initialize();


	cpuid(1,eax,ebx,ecx,edx);	
	int brand = ebx & 0xff;
	int stepping = eax & 0xf;
	int reserved = eax >> 14;
	int signature = eax;		
	cpuid(0x80000000,max_eax,unused,unused,unused);

	if(max_eax >= 0x80000004) 
	{
		kprintf("Brand: ");
		if(max_eax >= 0x80000002) {
			cpuid(0x80000002, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
		}
		if(max_eax >= 0x80000003) {
			cpuid(0x80000003, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
		}
		if(max_eax >= 0x80000004) {
			cpuid(0x80000004, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
		}
		
	} 
	else if(brand > 0) 
	{
		kprintf("Brand %d - \n", brand);
	} 
	else 
	{
		kprintf("Reserved  \n");
	}
	kprintf("Stepping: %d Reserved: %d  \n", stepping, reserved);

	u32 i = 0;
	multiboot_info_t *mbt;
	mbt = (multiboot_info_t *)addr;
	memory_map_t *mmap = (memory_map_t*)mbt->mmap_addr;
	u32 MapLim = mbt->mmap_addr + mbt->mmap_length;
	memory_map_t memMap[mbt->mmap_length / sizeof(memory_map_t)];

	/*while((u32)mmap < MapLim)
		{
			memMap[i].size = mmap->size;
			memMap[i].base_addr_low = mmap->base_addr_low;
			memMap[i].base_addr_high = mmap->base_addr_high;
			memMap[i].type = mmap->type;
			i++;
			mmap = (memory_map_t*)((u32)mmap + mmap->size + sizeof(u32));
		}*/
	for(;(u32)mmap < MapLim ;i++)
	{
		memMap[i].size = mmap->size;
			memMap[i].base_addr_low = mmap->base_addr_low;
			memMap[i].base_addr_high = mmap->base_addr_high;
			memMap[i].type = mmap->type;
			
			mmap = (memory_map_t*)((u32)mmap + mmap->size + sizeof(u32));
		
		kprintf(" size = 0x%x , base_addr = 0x%x%x , length = 0x%x%x , type = 0x%x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low,
					(unsigned) mmap->type);
    
    /* setup gdt */
	gdt();
	
	/* setup idt */
	idt();
	die();	
	}

}

void printregs(int eax, int ebx, int ecx, int edx) {
int j;
char string[17];
string[16] = '\0';
for(j = 0; j < 4; j++) {
	string[j] = eax >> (8 * j);
	string[j + 4] = ebx >> (8 * j);
	string[j + 8] = ecx >> (8 * j);
	string[j + 12] = edx >> (8 * j);
	}
kprintf("%s  ", string);
}
