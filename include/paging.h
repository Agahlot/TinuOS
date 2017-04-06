/* 

		  15              0    31                                    0
  LOGICAL +----------------+   +-------------------------------------+
  ADDRESS |    SELECTOR    |   |                OFFSET               |
          +---+---------+--+   +-------------------+-----------------+
       +------+         !                          |
       | DESCRIPTOR TABLE                          |
       |  +------------+                           |
       |  |            |                           |
       |  |            |                           |
       |  |            |                           |
       |  |            |                           |
       |  |------------|                           |
       |  |  SEGMENT   | BASE          +---+       |
       +->| DESCRIPTOR |-------------->| + |<------+
          |------------| ADDRESS       +-+-+
          |            |                 |
          +------------+                 |
          								 |
            		  +-----------+-----------+----------+         +---------------+
              		  |    DIR    |   PAGE    |  OFFSET  |         |               |
              		  +-----+-----+-----+-----+-----+----+         |               |
                    		|           |           |              |               |
    		  +-------------+           |           +------------->|    PHYSICAL   |
      		  |                         |                          |    ADDRESS    |
    		  |   PAGE DIRECTORY        |      PAGE TABLE          |               |
    		  |  +---------------+      |   +---------------+      |               |
    		  |  |               |      |   |               |      +---------------+
    		  |  |               |      |   |---------------|              ^
    		  |  |               |      +-->| PG TBL ENTRY  |--------------+
    		  |  |---------------|          |---------------|
    		  +->|   DIR ENTRY   |--+       |               |
    		     |---------------|  |       |               |
    		     |               |  |       |               |
    		     +---------------+  |       +---------------+
    		     ^                  |               ^
    +-------+        |          	|		|
    |  CR3  |--------+			+---------------+
    +-------+                 

*/

#include <mmu.h>
#include <boot.h>


#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR   1024
#define PAGE_SIZE       4096

#define PDE_SHIFT       22
#define PTE_SHIFT       12

#define PDE(va)         (((u32)(va) >> PDE_SHIFT) & 0x3FF)
#define PTE(va)         (((u32)(va) >> PTE_SHIFT) & 0x3FF)

/* construct a virtual address */
#define PGADDR(d, t, o) ((u32)((d) << PDE_SHIFT | (t) << PTE_SHIFT | (o)))

#define PTE_P           (1 << 0)   /* Present */
#define PTE_RW          (1 << 1)   /* Read/Write */

#define PDE_P           (1 << 0)   /* Present  */
#define PDE_RW          (1 << 1)   /* Read/Write */
#define PDE_PS          (1 << 7)   /* Page Size */

/* Page Table Entry */
typedef struct {
    unsigned present : 1;
    unsigned writable : 1;
    unsigned unaccessible : 1;
    unsigned write_through : 1;
    unsigned cache_disabled : 1;
    unsigned accessed : 1;
    unsigned dirty : 1;
    unsigned pat : 1;   /* Page Table Attribute Index */
    unsigned global_page : 1;
    unsigned avl : 3;
    unsigned frame_address : 20;
} __attribute__((packed)) pte_t;

/* Set PTE present bit */
#define SET_PTL_PRESENT_ARCH(ptl, i)
    set_pt_present((pte_t *) (ptl), (size_t (i)))

#define SET_PTL_FLAGS_ARCH(ptl, i, flags)
    set_pt_flags((pte_t *) (ptl), (size_t (i)), (flags))

#define GET_PTL_FLAGS_ARCH(ptl, i)
    get_pt_flags((pte_t *) (ptl), (size_t) (i))

static inline set_pt_present(pte_t *pt, size_t i)
{
    pte_t *p = &pt[i];
    p->present = 1;
}

static inline set_pt_flags(pte_t *pt, size_t i, int flags)
{
    pte_t *p = &pt[i];

    p->cache_disabled = !(flags & PAGE_CACHEABLE);
    p->present = !(flags & PAGE_NOT_PRESENT);
    p->unaccessible = (flags & PAGE_USER);
    p->writable = (flags & PAGE_WRITE);
    p->global_page = (flags & PAGE_GLOBAL);
}

static inline get_pt_flags(pte_t *pt, size_t i)
{
    pte_t *p = &pt[i];

    return ((!p->cache_disabled) << PAGE_CACHEABLE_SHIFT | 
          (!p->present) << PAGE_PRESENT_SHIFT | 
          (p->unaccessible) << PAGE_USER_SHIFT | 
          1 << PAGE_READ_SHIFT | 
          (p->writable) << PAGE_WRITE_SHIFT | 
          1 << PAGE_EXEC_SHIFT | 
          (p->global_page) << PAGE_GLOBAL_SHIFT);
}