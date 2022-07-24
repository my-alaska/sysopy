#include "library.h"
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static clock_t clock_start, clock_end;
static struct tms tms_start, tms_end;





Block ** createTable(int size){
    Block** blockArray = calloc(size,sizeof(Block*));
    return blockArray;
}





FILE* createTempFile(char ** files, int n){
    char command[1000] = "wc ";
    for(int i = 0; i < n; i++){
        strcat(command, files[i]);
        strcat(command, " ");
    }
    strcat(command, ">> ");
    char filename[] = "tmpXXXXXX";
    int descriptor = mkstemp(filename);
    FILE * tmp = fdopen(descriptor,"w+");
    strcat(command, filename);


    clock_start = times(&tms_start);

    system(command);

    clock_end = times(&tms_end);
    printf("\nWord Count Time\n");
    printf("real: %ld\n", clock_end - clock_start);
    printf("system: %ld\n", tms_end.tms_stime - tms_start.tms_stime);
    printf("user: %ld\n", tms_end.tms_utime - tms_start.tms_utime);


    unlink(filename);

    return tmp;
}





int blockCreate(FILE * tmp, Block ** blockArray, int n){
    int i = 0;
    for(;i<n;i++){
        if(blockArray[i]==NULL){
            break;
        }
    }
    if (i == n || blockArray[i] != NULL){
        return -1;
    }
    fseek(tmp, 0, SEEK_END);
    long length = ftell(tmp);
    fseek(tmp, 0, SEEK_SET);


    clock_start = times(&tms_start);

    char * buffer =  calloc(length+1,sizeof(int));
    int j=0;
    while(j<length){
        buffer[j] = fgetc(tmp);
        j++;
    }
    j++;
    buffer[j]='\0';
    blockArray[i] = calloc(1, sizeof(char *));
    blockArray[i] -> content = buffer;

    clock_end = times(&tms_end);
    printf("\nSave to Memory Time\n");
    printf("real: %ld\n", clock_end - clock_start);
    printf("system: %ld\n", tms_end.tms_stime - tms_start.tms_stime);
    printf("user: %ld\n", tms_end.tms_utime - tms_start.tms_utime);


    fclose(tmp);
    //printf("%s\n",blockArray[i]->content);
    return i;
}





int blockFree(Block ** blockArray,int i){

    clock_start = times(&tms_start);

    free(blockArray[i]);

    clock_end = times(&tms_end);
    printf("\nFree Memory Time\n");
    printf("real: %ld\n", clock_end - clock_start);
    printf("system: %ld\n", tms_end.tms_stime - tms_start.tms_stime);
    printf("user: %ld\n", tms_end.tms_utime - tms_start.tms_utime);


    blockArray[i] = NULL;

    return i;
}