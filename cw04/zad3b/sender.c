#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int received_sigusr1 = 0;
int received_sigusr2 = 0;
int catcher_received = 0;


void handler_1(int signal) {
    received_sigusr1++;
    return;
}

void handler_2(int signal) {
    received_sigusr2 = 1;
    return;
}

void handler_2_queue(int signal, siginfo_t *signal_info, void *v) {
    received_sigusr2 = 1;
    catcher_received = signal_info->si_value.sival_int;
    printf("%d\n",catcher_received);
    return;

}

int main(int argc, char ** argv){
    if(argc < 4) {
        return 1;
    }
    int catcher_pid = atoi(argv[1]);
    int no_signals = atoi(argv[2]);

    sigset_t mask;
    sigemptyset (&mask);



    if(strcmp(argv[3],"KILL")==0){
        signal(SIGUSR1, handler_1);
        signal(SIGUSR2, handler_2);

        for(int i = 0; i < no_signals; i++) {
            usleep(1000);
            kill(catcher_pid, SIGUSR1);
            sigsuspend(&mask);
//            pause();
        }
        kill(catcher_pid,SIGUSR2);

    }else if(strcmp(argv[3],"SIGQUEUE")==0){


        struct sigaction act;
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_2_queue;
        sigaction(SIGUSR2, &act, NULL);
        signal(SIGUSR1, handler_1);

        union sigval sval;
        sval.sival_int = 1;
        for(int i = 0; i < no_signals; i++) {
            usleep(1000);
            sigqueue(catcher_pid, SIGUSR1, sval);
            sigsuspend(&mask);
//            pause();
        }
        sigqueue(catcher_pid, SIGUSR2, sval);
        pause();

    }else if(strcmp(argv[3],"SIGRT")==0){
        signal(SIGRTMIN, handler_1);
        signal(SIGRTMAX, handler_1);

        for(int i = 0; i < no_signals; i++) {
            usleep(1000);
            kill(catcher_pid, SIGRTMIN);
            sigsuspend(&mask);
//            pause();
        }
        kill(catcher_pid, SIGRTMAX);
    }


    printf("Signals received(sender): %d\n",received_sigusr1);
    printf("Signals expected(sender): %d\n",no_signals);
    if( strcmp(argv[3], "SIGQUEUE")==0){
        printf("Signals found in catcher: %d\n",catcher_received);
    }
    return 0;
}
