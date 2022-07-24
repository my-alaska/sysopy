#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char ** argv){
    if(argc < 2) {
        return 1;
    }

    if(strcmp(argv[1], "ignore") == 0){
        raise(SIGUSR1);
    }else if(strcmp(argv[1], "mask") == 0){
        raise(SIGUSR1);
        sigset_t mask;
        sigpending(&mask);
        if (sigismember(&mask, SIGUSR1) == 1){
            printf("mask inherited. signal masked\n");
        }
    }else if(strcmp(argv[1], "pending") == 0) {
        sigset_t mask;
        sigpending(&mask);
        if (sigismember(&mask, SIGUSR1) == 1) {
            printf("signal is pending child process\n");
        } else {
            printf("signal isn't pending in child process\n");
        }
    }

    return 0;
}