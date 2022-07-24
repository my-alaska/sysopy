#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NO_REINDEERS 9
#define NO_ELVES 10
#define MIN_NO_PROBLEMS 3
#define GIFTS_2_DELIVER 3

pthread_t waiting_elves[MIN_NO_PROBLEMS];

pthread_t satan;
pthread_t elves[NO_ELVES];
pthread_t reindeers[NO_REINDEERS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t satan_c = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeers_c = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_c = PTHREAD_COND_INITIALIZER;
int no_waiting_reindeers = 0;
int no_waiting_elves = 0;
int gifts_delivered = 0;

void rand_sleep(int a, int b){
    a*=1000000;
    b*=1000000;
    usleep(a + rand() %(b-a));
}

void * run_satan(){
    int gifts_delivered = 0;
    while(gifts_delivered < GIFTS_2_DELIVER){
        pthread_mutex_lock(&mutex);
        while (no_waiting_reindeers < NO_REINDEERS && no_waiting_elves < MIN_NO_PROBLEMS){
            printf("Mikołaj: zasypiam\n");
            pthread_cond_wait(&satan_c, &mutex);
            printf("Mikołaj: budzę się\n");
        }


        if(no_waiting_reindeers == NO_REINDEERS){
            pthread_mutex_unlock(&mutex);
            printf("Mikołaj: dostarczam zabawki\n");
            rand_sleep(2,4);
            pthread_mutex_lock(&mutex);
            no_waiting_reindeers = 0;
            gifts_delivered++;
            pthread_cond_broadcast(&reindeers_c);
        }

        if(gifts_delivered == GIFTS_2_DELIVER){
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

        if(no_waiting_elves == MIN_NO_PROBLEMS){
            printf("Mikołaj: rozwiązuje problemy elfów %ld %ld %ld\n", waiting_elves[0],waiting_elves[1],waiting_elves[2]);

            for (int i = 0; i < MIN_NO_PROBLEMS; i++) {
                waiting_elves[i] = 0;
            }
            pthread_cond_broadcast(&elves_c);
            no_waiting_elves = 0;
            pthread_mutex_unlock(&mutex);
            rand_sleep(1,2);
            pthread_mutex_lock(&mutex);



        }




        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}




void * run_reindeer(){
    pthread_t id = pthread_self();
    while(1){
        rand_sleep(5,10);
        pthread_mutex_lock(&mutex);
        printf("Renifer: czeka %d reniferów na Mikołaja, %ld\n", no_waiting_reindeers, id);
        no_waiting_reindeers++;
        if(no_waiting_reindeers == NO_REINDEERS){
            printf("Renifer: wybudzam Mikołaja, %ld\n", id);
            pthread_cond_signal(&satan_c);
            pthread_cond_broadcast(&reindeers_c);
        }

        while (no_waiting_reindeers > 0) { // while na if
            pthread_cond_wait(&reindeers_c, &mutex);
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void * run_elf(){
//    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_t id = pthread_self();
    int waiting;
    while(1){
        waiting = 0;
        rand_sleep(2,5);
        pthread_mutex_lock(&mutex);
        if (no_waiting_elves == MIN_NO_PROBLEMS) {
            printf("Elf: czeka na powrót elfów, %ld\n",id);
            while(no_waiting_elves == MIN_NO_PROBLEMS){
                pthread_cond_wait(&elves_c, &mutex);
            }
        }
        if(no_waiting_elves < MIN_NO_PROBLEMS){
            printf("Elf: czeka %d elfów na Mikołaja, %ld\n", no_waiting_elves, id);
            waiting = 1;
            waiting_elves[no_waiting_elves] = id;
            no_waiting_elves++;
        }
        if(no_waiting_elves == MIN_NO_PROBLEMS){
            printf("Elf: wybudzam Mikołaja, %ld\n", id);
            pthread_cond_signal(&satan_c);
        }
        if(waiting){
            pthread_cond_wait(&elves_c, &mutex);
            printf("Elf: Mikołaj rozwiązuje problem, %ld\n", id);
        }

        pthread_mutex_unlock(&mutex);
    }
}

void exit_function(){
    pthread_join(satan, NULL);
    for(int i = 0; i < NO_REINDEERS; i++){
        pthread_cancel(reindeers[i]);
        pthread_join(reindeers[i], NULL);
    }
    for(int i = 0; i < NO_ELVES; i++){
        pthread_cancel(elves[i]); // for some reason this won't work with join

    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&satan_c);
    pthread_cond_destroy(&elves_c);
    pthread_cond_destroy(&reindeers_c);

}

void exit_handler(){
    pthread_cancel(satan);
    exit_function();
}

int main(int argc, char ** argv){
//    atexit(&exit_handler);

    pthread_create(&satan, NULL, run_satan, NULL);
    for (int i = 0; i < NO_ELVES; i++) pthread_create(&elves[i], NULL, run_elf, NULL);
    for (int i = 0; i < NO_REINDEERS; i++) pthread_create(&reindeers[i], NULL, run_reindeer, NULL);

    exit_function();

    return 0;
}

