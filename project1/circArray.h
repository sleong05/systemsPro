#ifndef CIRCARRAY_H
#define CIRCARRAY_H
#include <stdbool.h>
typedef struct {
    char **array;
    int index;
    int size;
} circArray;

circArray createCircArray(int size);

void insert(circArray *arr, char *line);

void printLastLines(circArray *arr, bool lineNumber, int curLine);

void deleteCircArray(circArray *arr);

#endif
