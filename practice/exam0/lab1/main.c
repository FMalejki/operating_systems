#include <stdio.h>
#include <stdlib.h>

#include "collatz.h"

int main(int argc, char** argv){
    if(argc != 3){
        return 1;
    }
    int number = atoi(argv[1]);
    int max_iter = atoi(argv[2]);
    int *steps = malloc(max_iter*sizeof(int));
    int res = test_collatz_convergence(number, max_iter, steps);
    if(res == 0){
        printf("%d", res);
    }
    else{
        for(int i = 0; i < max_iter && steps[i] != 1 ; i++){
            printf("%d\n",steps[i]);
        }
        printf("1\n");
    }
    return 0;
}