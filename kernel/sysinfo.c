#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_sysinfo(void){
    struct proc *p = myproc();
    struct sysinfo si;
    uint64 addr; // user pointer to struct sysinfo

    si =  (struct sysinfo){
        .freemem = get_total_free_bytes(),
        .nproc = get_nproc()
    };
    argaddr(0, &addr);
    if(copyout(p->pagetable, addr, (char *)&si, sizeof(si)) < 0)
        return -1;
    return 0;
}