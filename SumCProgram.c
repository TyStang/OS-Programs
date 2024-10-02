#include <stdio.h>
#include <stdlib.h>

int main() {
    int N; 
    int *array; 
    int sum = 0; 

    do {
        printf("Enter value of N [1-10]: ");
        scanf("%d", &N);
    } while (N < 1 || N > 10); 

    array = (int *)malloc(N * sizeof(int));

    if (array == NULL) {
        printf("Memory allocation failed!\n");
        return 1; 
    }

    printf("Enter %d integer number(s)\n", N);
    for (int i = 0; i < N; i++) {
        printf("Enter #%d: ", i + 1);
        scanf("%d", &array[i]);
    }

    for (int i = 0; i < N; i++) {
        sum += array[i];
    }

    printf("Sum: %d\n", sum);
    free(array);

    return 0; 
}
