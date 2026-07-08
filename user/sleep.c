#include "kernel/types.h"
#include "user/user.h"
int 
main(int argc, char *argv[]) 
{
    if(argc != 2) {
        fprintf(2, "usage: sleep [nr_ticks]\n");
        exit(1);
    }

    int nr_ticks = atoi(argv[1]);
    sleep(nr_ticks);
    exit(0);
}