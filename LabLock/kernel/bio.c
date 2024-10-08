// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

struct {
  struct spinlock lock[NBUCKET];
  struct buf buf[NBUF * NBUCKET];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head[NBUCKET];
} bcache;

static int hash(uint key)
{
  return key % NBUCKET;
}

void
binit(void)
{
  struct buf *b;
  char lockname[] = "bcache ", buffername[] = "buffer ";

  for (int i = 0; i < NBUCKET; i++){
    lockname[6] = buffername[6] = '0' + i;
    initlock(&bcache.lock[i], lockname);
    // Create linked list of buffers
    bcache.head[i].prev = &bcache.head[i];
    bcache.head[i].next = &bcache.head[i];
    for (b = bcache.buf + NBUF * i; b < bcache.buf + NBUF * (i + 1); b++) {
      b->next = bcache.head[i].next;
      b->prev = &bcache.head[i];
      initsleeplock(&b->lock, buffername);
      bcache.head[i].next->prev = b;
      bcache.head[i].next = b;
    }
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int bucket = hash(blockno);

  acquire(&bcache.lock[bucket]);

  // Is the block already cached?
  for(b = bcache.head[bucket].next; b != &bcache.head[bucket]; b = b->next) {
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.lock[bucket]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = bcache.head[bucket].prev; b != &bcache.head[bucket]; b = b->prev) {
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock[bucket]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  int bucket = hash(b->blockno);

  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.lock[bucket]);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.head[bucket].next;
    b->prev = &bcache.head[bucket];
    bcache.head[bucket].next->prev = b;
    bcache.head[bucket].next = b;
  }
  
  release(&bcache.lock[bucket]);
}

void
bpin(struct buf *b) {
  int bucket = hash(b->blockno);

  acquire(&bcache.lock[bucket]);
  b->refcnt++;
  release(&bcache.lock[bucket]);
}

void
bunpin(struct buf *b) {
  int bucket = hash(b->blockno);

  acquire(&bcache.lock[bucket]);
  b->refcnt--;
  release(&bcache.lock[bucket]);
}


