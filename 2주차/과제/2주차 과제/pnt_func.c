#include "main.h"

void pnt_func(int** k, int T) {

    for (int i = 0; i < T; i++) {
        for (int j = 0; j< 10; j++) {
            printf("%d ", k[i][j]);
        }
        printf("\n");
    }
}
