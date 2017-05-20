/* http://wiki.osdev.org/Brendan%27s_Memory_Management_Guide#Physical_Memory_Management */
/* TODO add buddy allocator */

#include <boot.h>
#include <kheap.h>

static header_t base;
static header_t *freep;

static char *heap_top, *heap_bottom, *g_kbrk;

u32 next_power_of_two(u32 const v)
{
  u32 r;

  if (v > 1)
  {
    float f = (float)v;
    u32 const t = 1UL << ((*(u32 *)&f >> 23) - 0x7f);
    r = t << (t < v);
  }
  else
  {
    r = 1;
  }
  return r;
}

void free(void *ap)
{
  header_t *m, *n;
  /* Find the location of the block which would be (block_address - sizeof(header_t)) */
  m = (header_t *)((char *)ap - sizeof(header_t));
  if (m->magic != HEAP_MAGIC)
  {
    kprintf("Free() is corrupted!\n");
    return;
  }

  /* Find the block in the Heap */
  n = (header_t *)heap_bottom;
  for (; n != NULL; n = n->next)
  {
    if (n == m)
      break;
  }
  if (n == NULL)
  {
    kprintf("Tried to free the memory at 0x");
    terminal_write_hex((u32)ap); 
    kprintf(" which does not exist!\n");
    return;
  }

  /* Mark the block as free */
  m->is_hole = 0;

  /* Delete that block from Heap and coalesing the free blocks */
  for (m = (header_t *)heap_bottom; m != NULL; m = m->next)
  {
    for (; m->is_hole == 0 && m->next->is_hole == 0 && m->next != NULL ;)
    {
      /* Calculate the block size */
      m->size += m->next->size + sizeof(header_t);

      /* Merge with the next block */
      m->next = m->next->next;
    }
  }

  //freep = (header_t *)((char *)ap - sizeof(header_t));
}

void *ksbrk(int allocate)
{ 
  char *new_brk, *old_brk;
  static char heap[MAX_HEAP];

  if (allocate == NULL)
  {
    kprintf("Could not allocate NULL block!\n");
    return NULL;
  }

  if (heap_bottom == NULL)
  {
    heap_bottom = g_kbrk = heap;
    heap_top = heap_bottom + MAX_HEAP;
    new_brk = heap_bottom + allocate;
  }

  if (heap_bottom != NULL)
  { 
    new_brk = g_kbrk + allocate;
  }

  if (heap_bottom >= new_brk)
    return NULL;

  if (new_brk >= heap_top)
    return NULL;

  /* Remenber the pervious allocations and allocate after that */
  old_brk = g_kbrk;
  g_kbrk = new_brk;

  return old_brk;
}


void*
malloc(u32 size)
{
  header_t *p, *m, *n;
  u32 total_size;
  int allocate;

  total_size = next_power_of_two(size) + sizeof(header_t);

  if (size == 0)
    return NULL;

  m = (header_t *)heap_bottom;

  if (m != NULL)
  {
    if (m->magic != HEAP_MAGIC)
    {
      kprintf("Malloc() is corrupted!\n");
      return NULL;
    }

    for (; m != NULL; m = m->next)
    {
      /* If block is used then, goto next block */
      if (m->is_hole == 1)
        continue;

      //if (size == m->size)
        //m->is_hole = 1;
     
      else 
      {
      /* If a block is not used, allocate a new block after the current block. 
       * For example block size to be allocated is 10 and size of header_t is 
       * 16 then, allocation should happen after m+16(size + sizeof(header_t)). 
       */
        n = (header_t *)((char *)m + total_size);
        n->size = m->size - total_size;
        n->magic = HEAP_MAGIC;
        n->next = m->next;
        n->is_hole = 0;

        /* Fill in the details of the block to be returned and point the block  
         * to n.
         */
        m->size = size;
        m->next = n;
        m->is_hole = 1;
      }
      /* The block after the header is returned */
      return (char *)m + sizeof(header_t);
    }
  }

    allocate = total_size;
    p = ksbrk(allocate);

    if (p == NULL)
      return NULL;

    p->size = size;
    p->magic = HEAP_MAGIC;
    p->is_hole = 1;

    if ((int)total_size == allocate)
      p->next = NULL;

  return (char *)p + sizeof(header_t);
}
