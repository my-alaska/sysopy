#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>


int sender_pid=0;
int received_sigusr1 = 0;
int received_sigusr2 = 0;

void handler_1(int signal, siginfo_t *signal_info, void *v){
    received_sigusr1++;
    if (sender_pid==0){
        sender_pid = signal_info->si_pid;
    }
}

void handler_2(int signal){
    received_sigusr2 = 1;
}



int main(int argc, char ** argv){
    if(argc < 2) {
        return 1;
    }
    printf("%d \n",getpid());

    if(strcmp(argv[1],"SIGRT")!=0){
        signal(SIGUSR2, handler_2);

        struct sigaction act;
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_1;
        sigaction(SIGUSR1, &act, NULL);
    }else{
        signal(SIGRTMAX, handler_2);

        struct sigaction act;
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_1;
        sigaction(SIGRTMIN, &act, NULL);
    }

    sigset_t mask;
    sigemptyset (&mask);
    while(received_sigusr2==0) {
        sigsuspend (&mask);
    }


    if(strcmp(argv[1],"KILL")==0){
        for(int i = 0; i < received_sigusr1; i++) {
            kill(sender_pid, SIGUSR1);
        }
        kill(sender_pid, SIGUSR2);

    }else if (strcmp(argv[1],"SIGQUEUE")==0){
        union sigval sval;
        for(int i = 0; i < received_sigusr1; i++) {
            sval.sival_int = i;
            sigqueue(sender_pid, SIGUSR1, sval);
        }
        sval.sival_int = received_sigusr1;
        sigqueue(sender_pid, SIGUSR2, sval);

    }else if(strcmp(argv[1],"SIGRT")==0){
        for(int i = 0; i < received_sigusr1; i++) {
            kill(sender_pid, SIGRTMIN);
        }
        kill(sender_pid, SIGRTMAX);
    }


    printf("Signals received(catcher): %d\n",received_sigusr1);

    return 0;
}
