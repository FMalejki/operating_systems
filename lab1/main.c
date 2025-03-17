#include <stdio.h>
#include "libcollatz"
#include <stdlib.h>


int main(){
    int input[] = {5,8,2,5,14};
    int input_size = 5;
    int max_iter = 30;
    int steps[max_iter];
    for(int i = 0; i < input_size; i++){
        int result = test_collatz_convergence(input[i], max_iter, steps);
        if( result != 0){
            printf("Input: %d\n", input[i]);
            printf("Sequence: ");
            for (int j = 0; j < result; j++) {
                printf("%d ", steps[j]);
            }
            printf("\n");
        } else {
            printf("Input: %d - Sequence not found in %d iterations! \n", input[i], max_iter);
        }
    }
}