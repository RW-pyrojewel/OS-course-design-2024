// reference counters with lock
struct rcounts
{
  struct spinlock lock;
  int counts[PHYSTOP / PGSIZE];
};
