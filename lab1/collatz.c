#include <stdio.h>
#include <stdlib.h>

int test_collatz_convergence(int input, int max_iter, int *steps)
{
    int result = input;
    int iter = 0;

    while (iter < max_iter) {
        steps[iter] = result;
        iter++;
        if (result == 1) {
            return iter;
        }
        result = collatz_conjecture(result);
    }
    
    return 0;
}

int collatz_conjecture(int input){
    int result;
    if( input % 2 == 0 ){
        return input / 2;
    }
    else{
        return 3 * input + 1;
    }
}