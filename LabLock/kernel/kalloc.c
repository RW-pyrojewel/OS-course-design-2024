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
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];

void
kinit()
{
  char lockname[] = "kmem ";

  for (int i = 0; i < NCPU; i++)
  {
    lockname[4] = '0' + i;
    initlock(&kmem[i].lock, lockname);
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  push_off();
  int ccpu = cpuid();   // current CPU ID
  pop_off();

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem[ccpu].lock);
  r->next = kmem[ccpu].freelist;
  kmem[ccpu].freelist = r;
  release(&kmem[ccpu].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int ccpu = cpuid();   // current CPU ID
  pop_off();

  acquire(&kmem[ccpu].lock);
  r = kmem[ccpu].freelist;

  // if freelist isn't empty, allocate one page, otherwise steal another CPU's free page.
  if(r)
    kmem[ccpu].freelist = r->next;
  else
    for (int i = 0; i < NCPU; i++)
      if (i != ccpu)
      {
        acquire(&kmem[i].lock);
        if ((r = kmem[i].freelist))
        {
          kmem[i].freelist = r->next;
          release(&kmem[i].lock);
          break;
        }
        else
          release(&kmem[i].lock);
      }

  release(&kmem[ccpu].lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
