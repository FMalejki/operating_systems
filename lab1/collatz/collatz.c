#include <stdio.h>
#include <stdlib.h>


int collatz_conjecture(int input){
    if( input % 2 == 0 ){
        return input / 2;
    }
    else{
        return 3 * input + 1;
    }
}

int test_collatz_conjecture(int input, int max_iter)
{
    int result = input;
    int iter = 0;

    while (iter < max_iter) {
        iter++;
        if (result == 1) {
            return iter;
        }
        result = collatz_conjecture(result);
    }
    
    return 0;
}
