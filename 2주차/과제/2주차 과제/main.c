#include <stdio.h>
#include <stdlib.h>

#include "main.h" // main.h 불러옴

int main() {

    int Test; // 실행 횟수
    scanf("%d", &Test); // 실행 횟수를 입력받는다

    int *k1 = (int*)malloc(sizeof(int) * Test); // 실행 횟수의 크기로 배열을 만든다. 페이지수를 저장할 배열.
    int **k2 = (int**)malloc(sizeof(int*) * Test); // 각 페이지수마다 0~9의 개수를 저장할 배열을 만든다.
    for (int i = 0; i < Test; i++) {
        k2[i] = (int*)malloc(sizeof(int) * 10); // 0~9까지, 총 10개의 int를 저장할 크기로 받는다.
    }

    for (int i = 0; i< Test; i++) {
        scanf("%d", &k1[i]); // k1 배열에 페이지수를 저장한다
    }

    for (int i = 0; i< Test; i++) {
        cal_func(k1[i], k2[i]); // cal_func을 실행시켜서 k1의 페이지수마다 숫자의 개수를 구하여 k2에 저장한다
    }
    
    pnt_func(k2, Test); // pnt_func을 실행시켜서 k2의 데이터를 출력한다.

    // malloc을 이용해 동적할당을 받았던 메모리를 해제한다.
    for (int i = 0; i< Test; i++) {
        free(k2[i]);
    }
    free(k1);
    free(k2);

    return 0;
}