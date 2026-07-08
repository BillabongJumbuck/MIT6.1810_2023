#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);
    if (fork() == 0) {
        char a;
        read(p[0], &a, sizeof a);
        fprintf(1, "%d: received ping\n", getpid());
        write(p[1], &a, sizeof a);
    } else {
        char a = 'a';
        write(p[1], &a, sizeof a);
        read(p[0], &a, sizeof a);
        fprintf(1, "%d: received pong\n", getpid());
    }
    exit(0);
}