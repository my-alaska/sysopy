#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <dirent.h>


static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;


char path[420];


int function(const char *lpath, const struct stat *buffer, int type){
    char * fullPath = realpath(lpath, NULL);
    printf("%s \n",fullPath);
    free(fullPath);
    printf("%ld - ", buffer->st_nlink);

    if (type == FTW_D){
        printf("directory ");
        ndir++;
    }else if(type == FTW_F) {
        switch(buffer->st_mode & S_IFMT){
            case S_IFREG:  printf("regular_file ");     nreg++;     break;
            case S_IFBLK:  printf("block_device ");     nblk++;     break;
            case S_IFCHR:  printf("character_device "); nchr++;     break;
            case S_IFIFO:  printf("FIFO/pipe ");        nfifo++;    break;
            case S_IFLNK:  printf("symlink ");          nslink++;   break;
            case S_IFSOCK: printf("socket ");           nsock++;    break;
            default:       printf("unknown ");                      break;
        }
    }
    printf("- %ld - ",buffer->st_size);

    printf("%s - ",strtok(ctime(&buffer->st_atime), "\n"));

    printf("%s\n",strtok(ctime(&buffer->st_mtime), "\n"));
    return 0;

}

int mynftw(){
    struct stat buffer;
    lstat(path,&buffer);
    lstat(path, &buffer);
    int fileType = S_ISDIR(buffer.st_mode);
    if(fileType){
        function(path, &buffer, FTW_D);
        unsigned long int length = strlen(path);
        path[length] = '/';
        length++;
        path[length] = '\0';
        DIR *dir = opendir(path);
        struct dirent *dirp;
        while (1) {
            dirp = readdir(dir);
            if (dirp == NULL){
                break;
            }
            if (strcmp(dirp->d_name, ".") != 0 && strcmp(dirp->d_name, "..") != 0){
                strcpy(&path[length], dirp->d_name);
                mynftw();
            }

        }
        length--;
        path[length] = '\0';
        closedir(dir);

    }else{
        function(path,&buffer,FTW_F);
    }



    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("usage: ftw <starting-pathname>");
    }
    strcpy(path,argv[1]);
    mynftw();
    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
    printf("regular_files =   %ld\n", nreg);
    printf("directories =     %ld\n", ndir);
    printf("block_special =   %ld\n", nblk);
    printf("char_special =    %ld\n", nchr);
    printf("FIFOs =           %ld\n", nfifo);
    printf("symbolic_links =  %ld\n", nslink);
    printf("sockets =         %ld\n", nsock);
    printf("total =           %ld\n",ntot);

    return 0;
}

