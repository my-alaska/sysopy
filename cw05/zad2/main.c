#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int print_sorted(char* arg){
    char* command = NULL;
    if(strcmp(arg,"data")==0){
        command = calloc(strlen("mail | sed '1d;$d' | sort -k5M -k6 -k7")+1,sizeof(char*));
        strcpy(command,"mail | sed '1d;$d' | sort -k5M -k6 -k7");
    }else if(strcmp(arg,"nadawca")==0){
        command = calloc(strlen("mail | sed '1d;$d' | sort -k3")+1,sizeof(char*));
        strcpy(command,"mail | sed '1d;$d' | sort -k3");

    }else{
        return 1;
    }

    FILE *file = popen(command, "w");
    if (file == NULL){
        puts("popopen fail\n");
        return 1;
    }
    pclose(file);
    free(command);
    return 0;
}

int send(char* address, char* title, char *text){
    char *command = calloc(strlen(address)+ strlen(title) +strlen(text) + 35, sizeof(char*));
    sprintf(command, "echo %s | mail %s -s %s", text, address, title);
    FILE* file = popen(command, "r");
    if (file == NULL){
        puts("popopen fail\n");
        return 1;
    }
    pclose(file);
    free(command);
    return 0;
}

int main(int argc, char ** argv){
    if (argc != 2 && argc != 4) {
        printf("There must be exactly one or exactly three arguments\n");
        return 1;
    }

    if(argc == 2){
        print_sorted(argv[1]);
    }else{
        send(argv[1],argv[2],argv[3]);
    }








    return 0;
}