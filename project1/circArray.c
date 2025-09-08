#include <stdlib.h>
#include <stdio.h>
typedef struct {
    char **array;
    int index;
    int size;
} circArray;

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
    arr->array[index] = line;
    arr->index = (arr->index+1)%arr->size;
}

void printLastLines(circArray *arr) {
    int index = arr->index;

    // print from the current index till the end of arr
    for (int i = index; i < arr->size;i++) {
        if (arr->array[i])
            printf("%s\n", arr->array[i]);

    }

    // print from the start of array till the index
    for (int i = 0; i < index;i++) {
        if (arr->array[i])
            printf("%s\n", arr->array[i]);
    }
}

void deleteCircArray(circArray *arr) {
    for (int i = 0; i<arr->size;i++) {
        if (arr->array[i])
            free(arr->array[i]);
    }
    free(arr->array);
}
