#ifndef UNTITLED_LIBRARY_H
#define UNTITLED_LIBRARY_H

#include <bits/types/FILE.h> //na pewno?

typedef struct {
    char* content;
}Block;
Block ** createTable(int);
FILE* createTempFile(char **, int);
int blockCreate(FILE *, Block **, int);
int blockFree(Block **, int);

#endif //UNTITLED_LIBRARY_H
