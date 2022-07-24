#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void handler(int a){
    printf("przechywcono sygnał\n");
    return;
}

int main(int argc, char ** argv){
    if(argc < 3) {
        return 1;
    }

    if(strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);

    }else if(strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, handler);
    }else if(strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &mask, NULL);
    }else{
        printf("błąd");
        return 1;
    }


    raise(SIGUSR1);



    if (strcmp(argv[1], "mask") == 0) {
        sigset_t mask;
        sigpending(&mask);
        if (sigismember(&mask, SIGUSR1) == 1){
            printf("signal is pending in parent process\n");
        }else{
            printf("signal isn't pending in parent process\n");
        }
    }




    if(strcmp(argv[2], "fork") == 0){
        pid_t pid = fork();
        if (pid == 0){

            if(strcmp(argv[1], "ignore") == 0 || strcmp(argv[1], "handler") == 0){
                raise(SIGUSR1);
            }else if(strcmp(argv[1], "mask") == 0){
                raise(SIGUSR1);
                sigset_t mask;
                sigpending(&mask);
                if (sigismember(&mask, SIGUSR1) == 1){
                    printf("mask inherited. signal masked\n");
                }
            }else if(strcmp(argv[1], "pending") == 0){
                sigset_t mask;
                sigpending(&mask);
                if (sigismember(&mask, SIGUSR1) == 1){
                    printf("signal is pending child process\n");
                }else{
                    printf("signal isn't pending in child process\n");
                }
            }
        }
        wait(NULL);
    }else if (strcmp(argv[1], "handler")!=0){
        execl("./program2", "./program2", argv[1], NULL);
    }

    return 0;
}