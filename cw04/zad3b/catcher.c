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
    return;
}

void handler_2(int signal, siginfo_t *signal_info, void *v){
    received_sigusr2 = 1;
    return;
}



int main(int argc, char ** argv){

    if(argc < 2) {
        return 1;
    }
    printf("%d\n",getpid());

    sigset_t mask;
    sigemptyset (&mask);

    if(strcmp(argv[1],"KILL")==0){
        struct sigaction act;
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_1;
        sigaction(SIGUSR1, &act, NULL);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_2;
        sigaction(SIGUSR2, &act, NULL);

        while(received_sigusr2 != 1){
            sigsuspend(&mask);
//            pause();
            if(received_sigusr2==1){
                break;
            }
            usleep(1000);
            kill(sender_pid, SIGUSR1);
        }


    }else if (strcmp(argv[1],"SIGQUEUE")==0){
        struct sigaction act;
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_1;
        sigaction(SIGUSR1, &act, NULL);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_2;
        sigaction(SIGUSR2, &act, NULL);

        union sigval sval;
        int i =0;

        while(received_sigusr2!=1) {
            sigsuspend(&mask);
//            pause();
            if(received_sigusr2==1){
                break;
            }
            usleep(1000);
            sval.sival_int = received_sigusr1;
            sigqueue(sender_pid, SIGUSR1, sval);
        }
        sval.sival_int = received_sigusr1;
        sigqueue(sender_pid, SIGUSR2, sval);

    }else if(strcmp(argv[1],"SIGRT")==0){
        struct sigaction act;
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_1;
        sigaction(SIGRTMIN, &act, NULL);
        act.sa_flags = SA_SIGINFO;
        act.sa_sigaction = handler_2;
        sigaction(SIGRTMAX, &act, NULL);

        while(received_sigusr2 != 1){
            sigsuspend(&mask);
//            pause();
            if(received_sigusr2==1){
                break;
            }
            usleep(1000);
            kill(sender_pid, SIGRTMIN);
        }
    }


    printf("Signals received(catcher): %d\n",received_sigusr1);

    return 0;
}
