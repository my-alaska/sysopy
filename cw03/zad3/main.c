#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

char path[420];
char strtarg[100];

int contains(char * filepath, char * string){
    FILE *fptr = fopen(filepath, "r");
    fseek (fptr, 0, SEEK_END);
    long int length = ftell(fptr);
    fseek (fptr, 0, SEEK_SET);
    char *buffer = malloc (length);
    fread (buffer, 1, length, fptr);
    fclose (fptr);

    char * pos = strstr(buffer, string);

    free(buffer);
    if(pos != NULL){
        return 1;
    }else{
        return 0;
    }
}


int curr(int depth,int maxdepth){
    depth++;
    if (depth>maxdepth){
        return 0;
    }


    int length = strlen(path);
    path[length] = '/';
    length++;
    path[length] = '\0';
    DIR *dir = opendir(path);
    struct dirent *dirp;
    struct stat buffer;
    int flag = 0;
    int children = 0;
    while (1) {
        dirp = readdir(dir);
        if (dirp == NULL){
            break;
        }

        if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0){
            strcpy(&path[length], dirp->d_name);
            lstat(path,&buffer);
            if ( S_ISDIR(buffer.st_mode) == 1) {
                flag = fork();
                if (flag == 0) {
                    curr(depth,maxdepth);
                    return 0;
                }else{
                    children++;
                }
            }else if (S_ISREG(buffer.st_mode) == 1) {
                if (contains(path, strtarg) == 1){
                    printf("%s %d\n",path,getpid());
                }
            }
        }

    }
    for(int i = 0 ; i < children; i++){
        wait(NULL);
    }



    return 0;
}


int main(int argc, char ** argv) {
    strcpy(path,argv[1]);

    strcpy(strtarg,argv[2]);

    int depth = 0;
    int maxdepth = atoi(argv[3]);

    curr(depth,maxdepth);
    return 0;
}
