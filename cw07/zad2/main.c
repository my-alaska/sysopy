#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PATHNAME "main"
#define PROJ_ID 2137
#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5

struct pizzeria {
    int oven[OVEN_CAPACITY];
    int table[TABLE_CAPACITY];
    int pizzas_in_oven;
    int pizzas_on_table;
};




int semaphore_id;
int memory_id;

sem_t *oven_taken;
sem_t *oven_slots;
sem_t *table_taken;
sem_t *table_slots;
sem_t *table_pizzas;
struct pizzeria * current_pizzeria;

int N, M;
pid_t * cooks = NULL;
pid_t * deliverers = NULL;
pid_t tmppid = -1;

time_t timer;
struct timeval cur_time;

void stop_function(int signal){
    exit(0);
}

int random_int(int a, int b){
    srand((int)time(&timer) % getpid());
    int r = rand();
    r = r%(b+1-a);
    r += a;
    return r;
}

void my_sleep(int a, int b){
    usleep(random_int(a*1000000,b*1000000));
}

void print_time(char ** buffer){
    gettimeofday(&cur_time, NULL);
    int milli = cur_time.tv_usec / 1000;

    *buffer = calloc(80,sizeof(char));
    strftime(*buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&cur_time.tv_sec));

    char current_time[5] = "";
    sprintf(current_time, ":%03d", milli);
    strcat(*buffer,current_time);
}

int pizzeria_init(){
    sem_unlink( "/oven_taken_f");
    sem_unlink( "/oven_slots_f");
    sem_unlink( "/table_taken_f");
    sem_unlink( "/table_slots_f");
    sem_unlink( "/table_pizzas_f");
    if((oven_taken = sem_open( "/oven_taken_f", O_CREAT,0666,1)) == SEM_FAILED){
        printf("shiiit\n");
    }
//    printf("%s \n", strerror(errno));
    oven_slots = sem_open( "/oven_slots_f", O_CREAT,0666,OVEN_CAPACITY);
    table_taken = sem_open( "/table_taken_f", O_CREAT,0666,1);
    table_slots = sem_open( "/table_slots_f", O_CREAT,0666,TABLE_CAPACITY);
    table_pizzas = sem_open( "/table_pizzas_f", O_CREAT,0666,0);

    memory_id = shm_open("/memory_f", O_RDWR | O_CREAT, 0666);
    ftruncate(memory_id, sizeof(struct pizzeria));
    current_pizzeria = mmap(NULL, sizeof(struct pizzeria), PROT_READ | PROT_WRITE, MAP_SHARED, memory_id, 0);

    for(int i = 0; i < OVEN_CAPACITY; i++) current_pizzeria->oven[i] = -1;
    for(int i = 0; i < TABLE_CAPACITY; i++) current_pizzeria->table[i] = -1;
    current_pizzeria->pizzas_in_oven = 0;
    current_pizzeria->pizzas_on_table = 0;

    munmap(current_pizzeria,sizeof(struct pizzeria));
    return 0;
}

int run_cook(pid_t pid){
    free(cooks);
    free(deliverers);

    sem_close( oven_taken);
    sem_close( oven_slots);
    sem_close( table_taken);
    sem_close( table_slots);
    sem_close( table_pizzas);
    oven_taken = sem_open("/oven_taken_f", 0);
    oven_slots = sem_open( "/oven_slots_f", 0);
    table_taken = sem_open( "/table_taken_f", 0);
    table_slots = sem_open( "/table_slots_f", 0);
    table_pizzas = sem_open( "/table_pizzas_f",0);

    memory_id = shm_open("/memory_f", O_RDWR, 0);
    current_pizzeria = mmap(NULL, sizeof(struct pizzeria), PROT_READ | PROT_WRITE, MAP_SHARED, memory_id, 0);


    int pizza_type;
    char *time_buffer;
    int place_in_oven = -1;
    int place_on_table = -1;

    while(1){
        pizza_type = random_int(0, 9);

        print_time(&time_buffer);
        printf("(%d %s) Przygotowuje pizze: %d\n", getpid(), time_buffer, pizza_type);
        free(time_buffer);
        my_sleep(1, 2);

        sem_wait(oven_slots);
        sem_wait(oven_taken);

        for(int i = 0; i < OVEN_CAPACITY; i++){
            if(current_pizzeria->oven[i] == -1){
                place_in_oven = i;
                break;
            }}
        current_pizzeria->oven[place_in_oven] = pizza_type;
        current_pizzeria->pizzas_in_oven++;

        print_time(&time_buffer);
        printf("(%d %s) Dodałem pizze: %d. Liczba pizz w piecu: %d.\n",getpid(),time_buffer,pizza_type,current_pizzeria->pizzas_in_oven);
        free(time_buffer);

        sem_post(oven_taken);



        my_sleep(4, 5);



        sem_wait(oven_taken);
        sem_wait(table_slots);
        sem_wait(table_taken);
        sem_post(table_pizzas);

        current_pizzeria->oven[place_in_oven] = -1;
        current_pizzeria->pizzas_in_oven--;
        for(int i = 0; i < TABLE_CAPACITY; i++){
            if(current_pizzeria->table[i] == -1){
                place_on_table = i;
                break;
            }}
        current_pizzeria->table[place_on_table] = pizza_type;
        current_pizzeria->pizzas_on_table++;

        print_time(&time_buffer);
        printf("(%d %s) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",getpid(),
               time_buffer,pizza_type,current_pizzeria->pizzas_in_oven,current_pizzeria->pizzas_on_table);
        free(time_buffer);

        sem_post(oven_taken);
        sem_post(table_taken);
        sem_post(oven_slots);
    }

}

int run_deliverer(pid_t pid){
    free(cooks);
    free(deliverers);

    sem_close( oven_taken);
    sem_close( oven_slots);
    sem_close( table_taken);
    sem_close( table_slots);
    sem_close( table_pizzas);
    oven_taken = sem_open( "/oven_taken_f", 0);
    oven_slots = sem_open( "/oven_slots_f", 0);
    table_taken = sem_open( "/table_taken_f", 0);
    table_slots = sem_open( "/table_slots_f", 0);
    table_pizzas = sem_open( "/table_pizzas_f",0);

    memory_id = shm_open("/memory_f", O_RDWR, 0);
    current_pizzeria = mmap(NULL, sizeof(struct pizzeria), PROT_READ | PROT_WRITE, MAP_SHARED, memory_id, 0);

    int pizza_type;
    int place_on_table;
    char *time_buffer;

    while(1){

        sem_wait(table_pizzas);
        sem_wait(table_taken);
        sem_post(table_slots);

        for(int i = 0; i < TABLE_CAPACITY; i++){
            if(current_pizzeria->table[i] != -1){
                place_on_table = i;
                break;
            }
        }
        pizza_type = current_pizzeria->table[place_on_table];
        current_pizzeria->table[place_on_table] = -1;
        current_pizzeria->pizzas_on_table--;

        print_time(&time_buffer);
        printf("(%d %s) Pobieram pizze: %d Liczba pizz na stole: %d.\n",getpid(),time_buffer,pizza_type,current_pizzeria->pizzas_on_table);
        free(time_buffer);

        sem_post(table_taken);


        my_sleep(4,5);

        print_time(&time_buffer);
        printf("(%d %s) Dostarczam pizze: %d.\n",getpid(),time_buffer,pizza_type);
        free(time_buffer);

        my_sleep(4,5);
    }

}

void handle_exit(){
    sem_close(oven_taken);
    sem_close(oven_slots);
    sem_close(table_taken);
    sem_close(table_slots);
    sem_close(table_pizzas);

    if(tmppid != 0){
        for(int i = 0; i < N; i++)kill(cooks[i],SIGINT);
        for(int i = 0; i < M; i++) kill(deliverers[i],SIGINT);

        sem_unlink("/oven_taken_f");
        sem_unlink("/oven_slots_f");
        sem_unlink("/table_taken_f");
        sem_unlink("/table_slots_f");
        sem_unlink("/table_pizzas_f");
        sem_unlink("/memory_f");
        free(cooks);
        free(deliverers);
    }else{
        munmap(current_pizzeria,sizeof(struct pizzeria));
    }
}

int main(int argc, char ** argv){
    if(argc != 3){
        printf("Wrong number of arguments\n");
    }
    signal(SIGINT, stop_function);
    atexit(handle_exit);



    pizzeria_init();



    N = atoi(argv[1]);
    cooks = calloc(N,sizeof(pid_t));
    for(int i = 0; i < N; i++){
        tmppid = fork();
        if (tmppid == 0){
            break;
        }else{cooks[i] = tmppid;}
    }
    if(tmppid == 0){
        return run_cook(tmppid);
    }

    M = atoi(argv[2]);
    deliverers = calloc(M,sizeof(pid_t));
    for(int i = 0; i < M; i++){
        tmppid = fork();
        if (tmppid == 0){
            break;}else{deliverers[i] = tmppid;
        }
    }
    if (tmppid == 0){
        return run_deliverer(tmppid);
    }


    for(int i = 0; i < N+M; i++){
        wait(NULL);
    }
    return 0;
}

