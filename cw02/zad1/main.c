#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "liblinerem.h"

int main(int argc, char ** argv){
    char *source = calloc(1,sizeof(char *)*20);
    char *target = calloc(1,sizeof(char *)*20);

    if(argc == 1){
        printf("Enter source file text: ");
        scanf("%s" , source);
        printf("Enter target file text: ");
        scanf("%s" , target);
    }else{
        strcpy(source,argv[1]);
        strcpy(target,argv[2]);
    }
    lib_line_remove(source,target);

    free(source);
    free(target);
    return 0;

}



