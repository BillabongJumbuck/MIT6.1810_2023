#include "kernel/types.h"
#include "user/user.h"

void subprocess(int rfd);

int 
main(int argv, char *argc[]) 
{
    int p[2];
    pipe(p);
    if (fork() == 0) {
        close(p[1]);
        subprocess(p[0]);
        exit(0);
    } else {
        close(p[0]);
        for (int i = 2; i <= 35; i++)
        {
            write(p[1], &i, sizeof i);
        }
        close(p[1]);
    }
    wait((int*)0);
    exit(0);
}

void subprocess(int rfd) 
{
    int this_prime;
    read(rfd, &this_prime, sizeof this_prime);
    fprintf(1, "prime %d\n", this_prime);
    int num;
    int is_first = 1;
    int p[2];
    pipe(p);
    while(read(rfd, &num, sizeof num) != 0) {
        if (num % this_prime != 0){
            if (is_first) {
                if(fork() == 0) {
                    close(rfd);
                    close(p[1]);
                    subprocess(p[0]);
                    return;
                } else {
                    is_first = 0;
                } 
            }
            write(p[1], &num, sizeof num);
        }
    }
    // close(p[0]);
    close(p[1]);
    // close(rfd);
    wait((int*)0);
}