#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>


void handler_a(int signal,siginfo_t *signal_info,void *v){
    printf("Signal number %d\n", signal_info->si_signo);
    printf("Original PID %d\n", signal_info->si_pid);
    printf("Signal code %d\n", signal_info->si_code);
}

void handler_b(int signal,siginfo_t *signal_info,void *v){
    printf("Signal number %d\n", signal_info->si_signo);
    printf("Original PID %d\n", signal_info->si_pid);
    printf("Signal status %d\n", signal_info->si_status);
}

void handler_c(int signal,siginfo_t *signal_info,void *v){
    printf("Signal number %d\n", signal_info->si_signo);
    printf("Original PID %d\n", signal_info->si_pid);
    printf("Signal value %d\n", signal_info->si_value.sival_int);
}

void handler_onstack(int signal){
    printf("sa_handler flag handler activates\n");
}

void handler_resethand(int signal){
    printf("sa_resethand flag handler activates\n");
}



int main(int argc, char ** argv){
    if(argc < 3) {
        return 1;
    }

    if(strcmp(argv[1], "siginfo") == 0){
        struct sigaction act;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_SIGINFO;
        if (strcmp(argv[2], "sigchld") == 0){
            act.sa_sigaction = handler_a;
            sigaction(SIGCHLD, &act, NULL);
            pid_t pid = fork();
            if (pid == 0){exit(1);}
            wait(NULL);
        }
        else if (strcmp(argv[2], "sigint") == 0){
            act.sa_sigaction = handler_b;
            sigaction(SIGINT, &act, NULL);
            kill(getpid(),SIGINT);
        }
        else if (strcmp(argv[2], "sigusr1") == 0){
            act.sa_sigaction = handler_c;
            sigaction(SIGUSR1, &act, NULL);
            raise(SIGUSR1);
        }

    }else if(strcmp(argv[1], "onstack") == 0){
        struct sigaction act;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_ONSTACK;
        act.sa_handler = handler_onstack;
        sigaction(SIGUSR1, &act, NULL);
        raise(SIGUSR1);
        printf("\n\n");

    }else if(strcmp(argv[1], "resethand") == 0){
        struct sigaction act;
        sigemptyset(&act.sa_mask);
        act.sa_flags = SA_RESETHAND;
        act.sa_handler = handler_onstack;
        sigaction(SIGUSR1, &act, NULL);
        raise(SIGUSR1);
        printf("\n\n");
    }

    printf("\n");
    return 0;
}
