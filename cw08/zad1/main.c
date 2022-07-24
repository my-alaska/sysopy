#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MAX_LINE_LEN 16384

enum program_mode{
    numbers,
    block
};

struct argument {
    int index;
};

enum program_mode mode;
char * input_name;
char * output_name;
int n;
int W, H, pixel_max_val;
unsigned char **image_data;
char file_head[72];

int get_image(char * name){
    FILE *image = fopen(name, "r");

    fgets(file_head, MAX_LINE_LEN, image);
    file_head[2] = '\0';

    char buffer[MAX_LINE_LEN];
    fgets(buffer,MAX_LINE_LEN,image);

    char * buffer2;
    buffer2 = strtok(buffer, " ");
    W = strtol(buffer2, NULL, 10);
    buffer2 = strtok(NULL, " ");
    H = strtol(buffer2,NULL,10);



    fgets(buffer, MAX_LINE_LEN, image);
    pixel_max_val = strtol(buffer, NULL, 10);


    image_data = calloc(H,sizeof(unsigned char *));

    buffer2 = NULL;
    for(int i = 0; i < H; i++){
        image_data[i] = calloc(W,sizeof(unsigned char));
        for(int j = 0; j < W; j++){
            buffer2 = strtok(NULL, " \t\r\n");
            if(buffer2 == NULL){
                fgets(buffer, MAX_LINE_LEN, image);

                buffer2 = strtok(buffer, " \t\r\n");
            }
            image_data[i][j] = strtol(buffer2,NULL,10);
        }
    }

    fclose(image);
    return 0;
}

int create_image(char * name){
    FILE * image = fopen(name, "w");

    fprintf(image, "%s\n", file_head);
    fprintf(image, "%d %d\n", W, H);
    fprintf(image, "%d", pixel_max_val);

    int in_line = 19;
    for(int i = 0; i < H; i++){
        for(int j=0; j < W; j++){
            if((W*i+j)%in_line == 0) fprintf(image,"\n");
            fprintf(image,"%d ", image_data[i][j]);
        }
    }
    fprintf(image,"\n");

    fclose(image);
    return 0;
}


void * proc_w_numbers(void * arg){
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);

    struct argument *argument = (struct argument *)arg;
    int index = argument->index;
    int whole = (W*H) / n;
    int remain = (W*H) % n;
    int counter, position;
    if(index <= remain){
        counter = whole;
        position = index*(whole+1);
    }else{
        counter = whole;
        position = remain*(whole+1) + (index-remain)*whole;
    }
    int i,j;

    i = position / W;
    j = position - i*W;
    unsigned char val;
    for(int k = 0; k < counter; k++){
        val = image_data[i][j];
        image_data[i][j] = pixel_max_val - val;
        j++;
        if(j >= W){
            j = 0;
            i++;
        }
    }

    clock_gettime(CLOCK_REALTIME, &stop);
    double thread_time = (double)(stop.tv_sec-start.tv_sec) + (double)(stop.tv_nsec-start.tv_nsec)/1000000000;
    printf("thread %d time: %f s\n",index,thread_time);
    return 0;
}

void * proc_w_block(void * arg){
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);

    struct argument *argument = (struct argument *)arg;
    int index = argument->index;
    int a = index*ceil(W/n);
    int b;
    if(index == n-1){
        b = W;
    }else{
        b = (index+1)*ceil(W/n);
    }

    unsigned char val;
    for(int i = a; i < b; i++){
        for(int j=0; j < H; j++){
            val = image_data[j][i];
            image_data[j][i] = pixel_max_val - val;
        }
    }


    clock_gettime(CLOCK_REALTIME, &stop);
    double thread_time = (double)(stop.tv_sec-start.tv_sec) + (double)(stop.tv_nsec-start.tv_nsec)/1000000000;
    printf("thread %d time: %f s\n",index,thread_time);
    return 0;
}




int main(int argc, char ** argv){



    n = atoi(argv[1]);
    printf("\nnumber of threads: %d \n",n);

    if(strcmp(argv[2],"numbers")==0) mode = numbers;
    else if(strcmp(argv[2],"block")==0) mode = block;
    else{printf("incorrect mode\n"); return 1;}



    input_name = calloc(strlen(argv[3])+1,sizeof(char));
    strcpy(input_name,argv[3]);

    output_name = calloc(strlen(argv[4])+1,sizeof(char));
    strcpy(output_name,argv[4]);




    get_image(input_name);

    void * fpointer;
    pthread_t *threads = calloc(n, sizeof(pthread_t));
    if(mode == numbers){
        fpointer = proc_w_numbers;
    }else if(mode == block){
        fpointer = proc_w_block;
    }


    struct argument * arguments = calloc(n,sizeof(struct argument));

    if(argc != 5){ printf("Wrong number of arguments\n"); return 1;}
    struct timespec start, stop;
    clock_gettime(CLOCK_REALTIME, &start);

    for(int i = 0; i < n; i++){
        arguments[i].index = i;
        pthread_create(&threads[i],NULL,fpointer,&arguments[i]);
    }
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &stop);
    double thread_time = (double)(stop.tv_sec-start.tv_sec) + (double)(stop.tv_nsec-start.tv_nsec)/1000000000;
    printf("full program time: %f s\n",thread_time);

    free(arguments);
    free(threads);




    create_image(output_name);

    free(input_name);
    free(output_name);
    for(int i = 0; i < H; i++) free(image_data[i]);
    free(image_data);


    return 0;
}

