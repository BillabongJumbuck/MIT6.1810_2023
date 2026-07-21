#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"

#ifndef LAB_PGTBL
#define LAB_PGTBL
#endif

#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  uint64 va;
  argaddr(0, &va);
  int nr_pages;
  argint(1, &nr_pages);
  uint64 pr_abits;
  argaddr(2, &pr_abits);

  if(nr_pages > 32) {
    return -1;      // we set the upper limit on the number of pages that can be scanned to 32.
  }
  
  uint32 bitmap = 0;
  pagetable_t pagetable = myproc() -> pagetable;

  for(int i=0; i < nr_pages; i++) {
    pte_t* pte = walk(pagetable, va, 0);
    if(pte == 0){
      return -1; // walk error
    }

    if(*pte & PTE_V){
      if(*pte & PTE_A){
        bitmap |= (1 << i);
        *pte &= (~PTE_A);
      }
    }
    va += PGSIZE;
  }

  if(copyout(pagetable, pr_abits, (char*)&bitmap, 4) < 0){
    return -1;
  };
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
