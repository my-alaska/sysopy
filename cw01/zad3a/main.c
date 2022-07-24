#ifdef DYNAMIC
#include <dlfcn.h>
#endif
#include "library.h"
#include <stdio.h>
#include <sys/times.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char ** argv){

    #ifdef DYNAMIC
    static void* handle = NULL;
    handle = dlopen("library.so", RTLD_NOW);
    if(handle == 0){
        printf("No handler\n");
        return 1;
    }
    Block** (*createTable)(int size) = dlsym(handle, "createTable");
    FILE* (*createTempFile)(char ** files, int n) = dlsym(handle,"createTempFile");
    int (*blockCreate)(FILE * tmp, Block ** blockArray, int n)=dlsym(handle,"blockCreate");
    int (*blockFree)(Block ** blockArray,int i)=dlsym(handle,"blockFree");

    #endif
















    if(strcmp(argv[1],"create_table") != 0){
        return 1;
    }

    Block ** blockArray = createTable(atoi(argv[2]));
    int j;
    int nFiles;
    FILE * tmp;


    for(int i = 3; i < argc; i++){
        if( strcmp(argv[i], "remove_block")==0){
            i++;
            blockFree(blockArray, atoi(argv[i]));
        }else if( strcmp(argv[i], "wc_files")==0){

            j = i;
            i++;

            while( i<argc && strcmp(argv[i], "remove_block")!=0 && strcmp(argv[i], "wc_files")!=0){
                i++;
            }
            nFiles= i-j-1;
            char *fileArray[nFiles];

            j++;
            for(int k = 0; k < nFiles; k++){
                fileArray[k] = argv[j];
                j++;
            }

            tmp = createTempFile(fileArray,nFiles);

            blockCreate(tmp, blockArray, atoi(argv[2]));
            i--;
        }
    }

    printf("\n\n\n");
    return 0;
}




