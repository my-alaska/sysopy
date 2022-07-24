#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int sys_line_remove(char * source, char * target){
    int fd1 = open(source,O_RDONLY);
    int fd2 = open(target,O_WRONLY);
    char * buffer = calloc(256,sizeof(char));
    int i = 1;
    int j,flag;
    int position = 0;
    i = read(fd1,buffer,sizeof(char)*256);
    ;
    while( i != 0 ){
        flag = j = 0;
        while(buffer[j] != '\n'){
            j++;
            if(buffer[j] != ' ' && buffer[j] != '\n'){
                flag = 1;
            }
        }
        if(flag == 1){
            write(fd2,buffer,sizeof(char)*(j+1));
        }
        position += (j+1);
        lseek(fd1,position,SEEK_SET);
        i = read(fd1,buffer,sizeof(char)*256);
    }
    free(buffer);
    return 0;
}

int lib_line_remove(char * source, char * target){
    FILE* fd1 = fopen(source,"r");
    FILE* fd2 = fopen(target,"w+");
    char * buffer = calloc(256,sizeof(char));
    int j,flag;

    fseek(fd1,0,SEEK_END);
    int eof = ftell(fd1);
    fseek(fd1,0,SEEK_SET);
    int position = ftell(fd1);

    while( position != eof){
        fread(buffer, 256*sizeof(char), 256, fd1);
        flag = j = 0;
        while(buffer[j] != '\n'){
            j++;
            if(buffer[j] != ' ' && buffer[j] != '\n'){
                flag = 1;
            }
        }
        if(flag == 1){
            fwrite(buffer,sizeof(char),j+1,fd2);
        }
        position += (j+1);
        fseek(fd1,position,SEEK_SET);
    }
    free(buffer);
    return 0;
}
