#define pragma once

#include <x86.h>

#define HEAP_MAGIC	0xdeadbeef	/* heap magic */
#define INDEX_SIZE	0x20000
#define MAX_HEAP	500000uL	/* max heap allocation */

typedef struct header {
	size_t size;
	struct header *next;
	unsigned magic : 32;
	unsigned is_hole : 1;
} header_t;