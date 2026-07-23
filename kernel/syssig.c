#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_sigalarm(void)
{
    struct proc* p = myproc();

    int ticks;
    argint(0, &ticks);
    uint64 handle_va;
    argaddr(1, &handle_va);

    p->alarm_interval = ticks;
    p->alarm_handle = (void (*)())handle_va;
    p->nr_ticks = 0;

    if(ticks != 0 && p->tf2 == 0) {
        if((p->tf2 = (struct trapframe *)kalloc()) == 0){
            return -1;
        }

        if(mappages(p->pagetable, TRAPFRAME2, PGSIZE,
            (uint64)(p->tf2), PTE_R | PTE_W) < 0){
            return -1;
        }
    }
    return 0;
}

uint64
sys_sigreturn(void)
{
    struct proc* p = myproc();
    *(p->trapframe) = *(p->tf2);
    p->alarm_nested = 0;
    return 0;
}