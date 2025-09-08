#include <stdlib.h>
#include <stdio.h>
#include "circArray.h"
#include <string.h>
#include <stdbool.h>

circArray createCircArray(int size){
    char **linesArray = calloc(size, sizeof(char *));
    circArray array = {linesArray, 0, size};

    return array;
}

void insert(circArray *arr, char *line) {
    int index = arr->index;
    if (arr->array[index]) {
        free(arr->array[index]);
    }
    arr->array[index] = strdup(line);
    arr->index = (arr->index+1)%arr->size;
}

void printLastLines(circArray *arr, bool lineNumber, int curLine) {
    int index = arr->index;
    int lineNum = curLine - arr->size -1;
    // print from the current index till the end of arr
    for (int i = index; i < arr->size;i++) {
        lineNum++;
        if (arr->array[i]) {
            printf("%d:", lineNum);
            printf("%s", arr->array[i]);
        }
    }

    // print from the start of array till the index
    for (int i = 0; i < index;i++) {
        lineNum++;
        if (arr->array[i]) {
            printf("%d:", lineNum);
            printf("%s", arr->array[i]);
        }
    }
}

void deleteCircArray(circArray *arr) {
    for (int i = 0; i<arr->size;i++) {
        if (arr->array[i])
            free(arr->array[i]);
    }
    free(arr->array);
}
