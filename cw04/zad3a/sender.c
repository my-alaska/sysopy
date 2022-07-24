#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int received_sigusr1 = 0;
int received_sigusr2 = 0;
int catcher_received = 0;
int rcatcher_received = 0;

void handler_1(int signal) {
    received_sigusr1++;
}

void handler_2(int signal) {
    received_sigusr2 = 1;
}

void handler_2_queue(int signal, siginfo_t *signal_info, void *v) {
    received_sigusr2 = 1;
    catcher_received = signal_info->si_value.sival_int;

}

int main(int argc, char ** argv){
    if(argc < 4) {
        return 1;
    }
    int catcher_pid = atoi(argv[1]);
    int no_signals = atoi(argv[2]);





    if(strcmp(argv[3],"KILL")==0){
        signal(SIGUSR1, handler_1);
        signal(SIGUSR2, handler_2);

        for(int i = 0; i < no_signals; i++) {
            kill(catcher_pid, SIGUSR1);
        }
        kill(catcher_pid, SIGUSR2);
    }else if(strcmp(argv[3],"SIGQUEUE")==0){
        signal(SIGUSR1, handler_1);
        struct sigaction act;
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_2_queue;
        sigaction(SIGUSR2, &act, NULL);

        union sigval sval;
        sval.sival_int = 0;
        for(int i = 0; i < no_signals; i++) {
//            sval.sival_int = i;
            sigqueue(catcher_pid, SIGUSR1, sval);
        }
//        sval.sival_int = no_signals;
        sigqueue(catcher_pid, SIGUSR2, sval);

    }else if(strcmp(argv[3],"SIGRT")==0){
        signal(SIGRTMIN, handler_1);
        signal(SIGRTMAX, handler_2);

        for(int i = 0; i < no_signals; i++) {
            kill(catcher_pid, SIGRTMIN);
        }
        kill(catcher_pid, SIGRTMAX);
    }


    sigset_t mask;
    sigemptyset (&mask);


    while (received_sigusr2==0){
        sigsuspend (&mask);
    }




    printf("Signals received(sender): %d\n",received_sigusr1);
    printf("Signals expected(sender): %d\n",no_signals);
    if( strcmp(argv[3], "SIGQUEUE")==0){
        printf("Signals found in catcher: %d\n",catcher_received);
    }
    return 0;
}
