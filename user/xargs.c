#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

char *params[MAXARG]; 

void 
fork1()
{
    int pid = fork();
    if (pid == 0) {
        exit(exec(params[0], params) == 0 ? 0 : 1);
    } else if (pid > 0) {
        wait((int*)0);
    } else {
        fprintf(2, "xargs: can not create new process!");
        return;
    }
}
int 
main(int argc, char *argv[]) 
{
    if (argc > MAXARG) {
        fprintf(2, "too many args.");
        exit(1);
    }

    int i;
    for(i = 1; i < argc; i++) {
        params[i-1] = argv[i];
    }
    
    char buf[512];
    params[i-1] = buf;
    char *p = buf;

    char c;
    while((read(0, &c, sizeof c)) > 0) {
        if(c == '\n') {
            if(p != buf) { // skip the empty line
                *p = '\0';
                fork1();
                p = buf;
            }
        } else {
            // check before write, reserve one byte for '\0'
            if (p >= buf + sizeof(buf) - 1) {
                fprintf(2, "xargs: args too long!");
                exit(1);
            }
            *p = c;
            p++;
        }
    }
    // edge case：no '\n' on the last line.
    if (p > buf) {
        if (p < buf + sizeof(buf)) {
            *p = '\0';
            fork1();
        } else {
            fprintf(2, "xargs: args too long!");
        }
    }
    exit(0);
}