#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "liblinerem.h"

int main(int argc, char ** argv){
    char *source = calloc(1,sizeof(char *)*20);
    char letter;

    if(argc == 1){
        printf("Enter your letter: ");
        scanf("%c" , &letter);
        printf("Enter source file text: ");
        scanf("%s" , source);

    }else{
        strcpy(source,argv[2]);
        letter = *argv[1];
    }

    lib_custom_wc(source,letter);

    free(source);

    return 0;

}



