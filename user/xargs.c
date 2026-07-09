#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"
int 
main(int argc, char *argv[]) 
{
    if (argc > MAXARG) {
        fprintf(2, "too many args.");
        exit(1);
    }

    int i;
    char *params[MAXARG];  // suppose that each args no longer than 31
    for(i = 1; i < argc; i++) {
        params[i-1] = argv[i];
    }

    i--;
    
    char buf[512];
    char *p = buf;

    int n = 0;
    int n_acc = 0;
    while((n = (read(0, buf + n_acc, sizeof(buf) - n_acc))) > 0){
       n_acc += n;
    }
    if (n < 0) {
         fprintf(2, "xargs: failed to read from stdin!\n");
    }

    while (p < buf + n_acc) {
        while(*p == ' ' || *p == '\n') {
            p++;
        }
        params[i++] = p;
        if (i >= MAXARG) {
            fprintf(2, "xargs: cmd being execed has too many args!");
            exit(1);
        }
        while(*p != ' ' && *p != '\n') {
            p++;
        }
        *p = '\0';
        p++;
    } 
    

    exec(params[0], params);
    exit(0);
}