#include <stdio.h>
#include <stdlib.h>

#define MALLOC_SIZE 10

int main () {
    int *mat = malloc(MALLOC_SIZE * sizeof(*mat));
    if (!mat) return 1;
    
    for (int i = 0; i < MALLOC_SIZE; i++) {
        mat[i] = rand();
    }

    for (int i = 0; i < MALLOC_SIZE; i++) {
        printf("%d\n", mat[i]);
    }

    free(mat);
    return 0;
}

