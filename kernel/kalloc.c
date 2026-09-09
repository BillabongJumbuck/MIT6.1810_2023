// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock[NCPU];
  struct run *freelist[NCPU];
} kmem;

void
kinit()
{
  char *start = (char*)PGROUNDUP((uint64)end);
  uint64 step = (((uint64)PHYSTOP - (uint64)start) / NCPU) & ~(PGSIZE-1);
  for(int i=0; i < NCPU; i++) {
    initlock(&kmem.lock[i], "kmem");

    char* list_start = start + i * step;
    char* list_end = start + (i + 1) * step;
    if (i + 1 >= NCPU) {
      list_end = (char*)PHYSTOP;
    }

    for(char *p = list_start; p + PGSIZE < list_end; p += PGSIZE) {
      // kree
      memset(p, 1, PGSIZE);
      struct run *r = (struct run*)p;
      r->next = kmem.freelist[i];
      kmem.freelist[i] = r;
    }
  }
  printf("init end!\n");
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off();
  int hartid = cpuid();
  acquire(&kmem.lock[hartid]);
  r->next = kmem.freelist[hartid];
  kmem.freelist[hartid] = r;
  release(&kmem.lock[hartid]);
  pop_off();
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int hartid = cpuid();
  acquire(&kmem.lock[hartid]);
  r = kmem.freelist[hartid];
  if(r)
    kmem.freelist[hartid] = r->next;
  release(&kmem.lock[hartid]);

  if (r == 0) {
    // steal
    for(int i =  (hartid + 1) % NCPU; i != hartid; i = (i + 1) % NCPU) {
      // printf("%d steal %d\n", hartid, i);
      acquire(&kmem.lock[i]);
      r = kmem.freelist[i];
      if(r)
        kmem.freelist[i] = r->next;
      release(&kmem.lock[i]);
      if(r)
        break;
    }
  }

  pop_off();

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
