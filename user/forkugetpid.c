#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main()
{
    int pid1 = getpid();
    int uid1 = ugetpid();
    if (pid1 != uid1) {
        printf("FAIL: parent getpid=%d, ugetpid=%d\n", pid1, uid1);
        exit(1);
    }

    int f = fork();
    if (f < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (f == 0) {
        // 子进程
        int pid2 = getpid();
        int uid2 = ugetpid();
        if (pid2 != uid2) {
            printf("FAIL: child getpid=%d, ugetpid=%d\n", pid2, uid2);
            exit(1);
        }
        printf("child OK: pid=%d, ugetpid=%d\n", pid2, uid2);
        exit(0);
    } else {
        // 父进程
        wait(0);
        // 再次检查父进程（确认没被破坏）
        int pid3 = getpid();
        int uid3 = ugetpid();
        if (pid3 != uid3) {
            printf("FAIL: parent after wait getpid=%d, ugetpid=%d\n", pid3, uid3);
            exit(1);
        }
        printf("parent OK: pid=%d, ugetpid=%d\n", pid3, uid3);
    }

    printf("forkugetpid_test: OK\n");
    exit(0);
}