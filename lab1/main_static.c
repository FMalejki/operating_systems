#include <stdio.h>
#include "collatz/collatz.h"

int main(){
    printf("%d\n", collatz_conjecture(10));
    printf("%d\n", test_collatz_conjecture(10, 100));
}
