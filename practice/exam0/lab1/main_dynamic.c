#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include "collatz.h"

int main(int argc, char** argv){
    void* handle = dlopen("./build/libcollatz_shared_lib.so", RTLD_LAZY);
    if(handle == NULL){
        return 1;
    }
    int (*lib_function)(int, int, int*);
    lib_function = (int(*)())dlsym(handle, "test_collatz_convergence");

    if(dlerror() != NULL){
        return 1;
    }
    if(argc != 3){
        return 1;
    }
    int number = atoi(argv[1]);
    int max_iter = atoi(argv[2]);
    int *steps = malloc(max_iter*sizeof(int));
    int res = (*lib_function)(number, max_iter, steps);
    if(res == 0){
        printf("%d", res);
    }
    else{
        for(int i = 0; i < max_iter && steps[i] != 1 ; i++){
            printf("%d\n",steps[i]);
        }
        printf("1\n");
    }
    dlclose(handle);
    return 0;
}