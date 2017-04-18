#include <boot.h>
#include <x86.h>

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
u8 attrib;
u16* terminal_buffer;

static void move_cursor()
{
	u16 cursorLocation = terminal_row * VGA_WIDTH + terminal_column;
	outb(14, 0x3D4);
	outb(cursorLocation >> 8, 0x3D5);
	outb(15, 0x3D4);
	outb(cursorLocation, 0x3D5);
}

void scroll()
{
	unsigned blank, temp;
	attrib = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	/* blank is defined as space + black color */
	blank = vga_entry(0x20, attrib);

	if (terminal_row >= VGA_HEIGHT)
	{
		int i;
		for (i = 0*80; i < 24*80; i++)
        {
            terminal_buffer[i] = terminal_buffer[i+80];
        }

        // The last line should now be blank. Do this by writing
        // 80 spaces to it.
        for (i = 24*80; i < 25*80; i++)
        {
            terminal_buffer[i] = blank;
        }
        // The cursor should now be on the last line.
        terminal_row = 24;
    }	
}
 
void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	attrib = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (u16*) 0x800B8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', attrib);
		}
	}
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

void terminal_setcolor(u8 color) {
	attrib = color;
}
 
void terminal_putchar(char c) {
	/* Handle backspace, by moving the cursor back one space */
	if (c == 0x08)
	{	
		if (terminal_row != 0)
			terminal_row--;
	}

	else if (c == '\r')
	{
		terminal_row = 0;
	}

	else if (c == '\n')
	{
		terminal_column = 0;
		terminal_row++;
	}

	else if (c >= ' ')
	{
		//terminal_column++;
	}

	if (terminal_row >= 80)
	{
		terminal_column = 0;
		terminal_row++;
	}

	terminal_putentryat(c, attrib, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT)
			terminal_row += 1;
	}

	move_cursor();
	scroll();
}
 
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_write_hex(u32 n)
{
    u32 tmp;

    char noZeroes = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }
    
        if (tmp >= 0xA)
        {
            noZeroes = 0;
            terminal_putchar(tmp-0xA+'a' );
        }
        else
        {
            noZeroes = 0;
            terminal_putchar( tmp+'0' );
        }
    }
  
    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        terminal_putchar(tmp-0xA+'a');
    }
    else
    {
        terminal_putchar(tmp+'0');
    }

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