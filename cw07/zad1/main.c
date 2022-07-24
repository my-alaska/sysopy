#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

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

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};




key_t key;
int semaphore_id;
int memory_id;
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
    key = ftok(PATHNAME, PROJ_ID);
    semaphore_id = semget(key, 5, IPC_CREAT | 0666);
    union semun *arg = calloc(1,sizeof(union semun));
    arg->val = 1;
    semctl(semaphore_id, 0, SETVAL, *arg); // is oven in_server
    arg->val = OVEN_CAPACITY;
    semctl(semaphore_id, 1, SETVAL, *arg); // oven empty slots
    arg->val = 1;
    semctl(semaphore_id, 2, SETVAL, *arg); // is table in_server
    arg->val = TABLE_CAPACITY;
    semctl(semaphore_id, 3, SETVAL, *arg); // table empty slots
    arg->val = 0;
    semctl(semaphore_id, 4, SETVAL, *arg); // pizzas on the table
    free(arg);



    memory_id = shmget(key, sizeof(struct pizzeria), IPC_CREAT | 0666);
    current_pizzeria = shmat(memory_id, NULL, 0);
    for(int i = 0; i < OVEN_CAPACITY; i++) current_pizzeria->oven[i] = -1;
    for(int i = 0; i < TABLE_CAPACITY; i++) current_pizzeria->table[i] = -1;
    current_pizzeria->pizzas_in_oven = 0;
    current_pizzeria->pizzas_on_table = 0;
    shmdt(current_pizzeria);
    return 0;
}

int run_cook(pid_t pid){
    free(cooks);
    free(deliverers);

    semaphore_id = semget(key,0,0);
    memory_id = shmget(key,0,0);
    current_pizzeria = shmat(memory_id, NULL, 0);



    struct sembuf oven_taken;
    struct sembuf oven_slots;
    struct sembuf table_taken;
    struct sembuf table_slots;
    struct sembuf table_pizzas;
    oven_taken.sem_num = 0;
    oven_slots.sem_num = 1;
    table_taken.sem_num = 2;
    table_slots.sem_num = 3;
    table_pizzas.sem_num = 4;
    oven_taken.sem_flg = 0;
    oven_slots.sem_flg = 0;
    table_taken.sem_flg = 0;
    table_slots.sem_flg = 0;
    table_pizzas.sem_flg = 0;

    int pizza_type;
    char *time_buffer;
    int place_in_oven = -1;
    int place_on_table = -1;

    struct sembuf s_buffer[4];

    while(1){
        pizza_type = random_int(0, 9);

        print_time(&time_buffer);
        printf("(%d %s) Przygotowuje pizze: %d\n", getpid(), time_buffer, pizza_type);
        free(time_buffer);
        my_sleep(1, 2);



        oven_taken.sem_op = -1;
        oven_slots.sem_op = -1;
        s_buffer[0] = oven_taken;
        s_buffer[1] = oven_slots;
        semop(semaphore_id, s_buffer, 2);

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

        oven_taken.sem_op = 1;
        s_buffer[0] = oven_taken;
        semop(semaphore_id, s_buffer, 1);
        my_sleep(4, 5);



        oven_taken.sem_op = -1;
        table_taken.sem_op = -1;
        table_slots.sem_op = -1;
        table_pizzas.sem_op = 1;
        s_buffer[0] = oven_taken;
        s_buffer[1] = table_taken;
        s_buffer[2] = table_slots;
        s_buffer[3] = table_pizzas;
        semop(semaphore_id, s_buffer, 4);

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

        oven_taken.sem_op=1;
        table_taken.sem_op=1;
        oven_slots.sem_op=1;
        s_buffer[0] = oven_taken;
        s_buffer[1] = table_taken;
        s_buffer[2] = oven_slots;
        semop(semaphore_id, s_buffer, 3);
//        break;
    }

    return 0;
}

int run_deliverer(pid_t pid){
    free(cooks);
    free(deliverers);

    semaphore_id = semget(key,0,0);
    memory_id = shmget(key,0,0);
    current_pizzeria = shmat(memory_id, NULL, 0);

    struct sembuf oven_taken;
    struct sembuf oven_slots;
    struct sembuf table_taken;
    struct sembuf table_slots;
    struct sembuf table_pizzas;
    oven_taken.sem_num = 0;
    oven_slots.sem_num = 1;
    table_taken.sem_num = 2;
    table_slots.sem_num = 3;
    table_pizzas.sem_num = 4;
    oven_taken.sem_flg = 0;
    oven_slots.sem_flg = 0;
    table_taken.sem_flg = 0;
    table_slots.sem_flg = 0;
    table_pizzas.sem_flg = 0;

    int pizza_type;
    int place_on_table;
    char *time_buffer;

    struct sembuf s_buffer[2];

    while(1){

        table_pizzas.sem_op = -1;
        table_taken.sem_op = -1;
        table_slots.sem_op = 1;
        s_buffer[0] = table_pizzas;
        s_buffer[1] = table_taken;
        s_buffer[2] = table_slots;
        semop(semaphore_id, s_buffer, 3);

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

        table_taken.sem_op = 1;
        s_buffer[0] = table_taken;
        semop(semaphore_id, s_buffer, 1);


        my_sleep(4,5);

        print_time(&time_buffer);
        printf("(%d %s) Dostarczam pizze: %d.\n",getpid(),time_buffer,pizza_type);
        free(time_buffer);

        my_sleep(4,5);
    }
    return 0;
}

void handle_exit(){
    if(tmppid != 0){
        for(int i = 0; i < N; i++)kill(cooks[i],SIGINT);
        for(int i = 0; i < M; i++) kill(deliverers[i],SIGINT);
        semctl(semaphore_id, 0, IPC_RMID);
        shmctl(memory_id, IPC_RMID, NULL);
        free(cooks);
        free(deliverers);
    }else{
        shmdt(current_pizzeria);
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

