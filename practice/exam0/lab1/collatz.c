#include <stdio.h>
#include <stdlib.h>
#include "collatz.h"

int collatz_conjecture(int number){
    if(number % 2 == 0){
        return number/2; 
    } 
    else{
        return number * 3 + 1;
    }
}

int test_collatz_convergence(int input, int max_iter, int *steps){
    for(int i = 0; i < max_iter; i++){
        int result = collatz_conjecture(input);
        steps[i] = result;
        if(result == 1){
            return 1;
        }
        input = result;
    }
    return 0;
}