#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>






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

    system(command);

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

    fclose(tmp);
    //printf("%s\n",blockArray[i]->content);
    return i;
}





int blockFree(Block ** blockArray,int i){

    free(blockArray[i]);

    blockArray[i] = NULL;

    return i;
}