#include <x86.h>

#define HEAP_MAGIC	0xdeadbeef
#define INDEX_SIZE	0x20000

typedef void *type_t;

/* lessthan_t always returns a nonzero argument is first argument
 * is less than second.
 */
typedef u8 (*lessthan_t)(type_t, type_t);

typedef struct {
	u32 magic;
	u8 is_hole;
	u32 size;
} header_t;

typedef struct {
	u32 magic;
	header_t *header;
} footer_t;

typedef struct {
	type_t *array;	/* This array always remains in sorted state */
	u32 size;
	u32 max_size;
	lessthan_t less_than;
} ordered_array_t;

typedef struct {
	ordered_array_t index;
	u32 start_address;
	u32 end_adddress;
	u32 max_address;
} heap_t;

/* Create a heap */
heap_t *create_heap(u32 start, u32 end, u32 max);

/* Free a heap */
void free(type_t *p, heap_t *heap);