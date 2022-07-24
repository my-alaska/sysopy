#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int sys_custom_wc(char * source, char letter){
    int fd1 = open(source,O_RDONLY);
    char * buffer = calloc(256,sizeof(char));

    int i = 1;
    int j;
    int position = 0;

    i = read(fd1,buffer,sizeof(char)*256);

    int flag;
    int overall = 0;
    int lines = 0;
    while( i != 0 ){
        flag = j = 0;
        while(buffer[j] != '\n'){
            if(buffer[j] == letter){
                flag = 1;
                overall++;
            }
            j++;
        }
        if(flag == 1){
            lines++;
        }
        position += (j+1);
        lseek(fd1,position,SEEK_SET);
        i = read(fd1,buffer,sizeof(char)*256);
    }
    free(buffer);
    printf("overall: %d\n lines: %d\n",overall, lines);
    return 0;
}

int lib_custom_wc(char * source, char letter){
    FILE* fd1 = fopen(source,"r");
    char * buffer = calloc(256,sizeof(char));
    int j,flag;

    fseek(fd1,0,SEEK_END);
    int eof = ftell(fd1);
    fseek(fd1,0,SEEK_SET);
    int position = ftell(fd1);

    int overall = 0;
    int lines = 0;
    while( position != eof){
        fread(buffer, 256*sizeof(char), 256, fd1);
        flag = j = 0;
        while(buffer[j] != '\n'){
            j++;
            if(buffer[j] == letter){
                overall++;
                flag = 1;
            }
        }
        if(flag == 1){
            lines++;
        }
        position += (j+1);
        fseek(fd1,position,SEEK_SET);
    }
    free(buffer);
    printf("overall: %d\n lines: %d\n",overall, lines);
    return 0;
}
