#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <ftw.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int function(const char *path, const struct stat *buffer, int type, struct FTW *ftw){
    char * fullPath = realpath(path,NULL);
    printf("%s \n",fullPath);
    free(fullPath);
    printf("%ld - ", buffer->st_nlink);

    if (type == FTW_D){
        printf("directory ");
        ndir++;
    }else if(type == FTW_F) {
        switch(buffer->st_mode & S_IFMT){
            case S_IFREG:  printf("regular_file ");    nreg++;     break;
            case S_IFBLK:  printf("block_device ");    nblk++;     break;
            case S_IFCHR:  printf("character_device ");nchr++;     break;
            case S_IFIFO:  printf("FIFO/pipe ");       nfifo++;    break;
            case S_IFLNK:  printf("symlink ");         nslink++;   break;
            case S_IFSOCK: printf("socket ");          nsock++;    break;
            default:       printf("unknown ");                     break;
        }
    }
    printf("- %ld - ",buffer->st_size);

    printf("%s - ",strtok(ctime(&buffer->st_atime), "\n"));

    printf("%s\n",strtok(ctime(&buffer->st_mtime), "\n"));
    return 0;

}

int main(int argc, char *argv[]) {
    if (argc != 2){
        printf("usage: ftw <starting-pathname>");
    }

    nftw(argv[1],  function, 10, FTW_PHYS);
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