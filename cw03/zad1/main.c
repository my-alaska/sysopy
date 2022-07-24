#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char ** argv) {
    if (argc != 2){
        printf("invalid number of arguments");
        return 1;
    }
    int n = atoi(argv[1]);

    pid_t child_pid;

    for(int i = 0; i < n; i++){
        child_pid = fork();
        if(child_pid==0){
            printf("notice from %d process\n", (int)getpid());
            break;
        }
    }
    for(int i = 0; i < n; i++){
        wait(NULL);
    }

    return 0;
}