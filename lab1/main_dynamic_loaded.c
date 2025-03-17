#include <stdio.h>
#include <dlfcn.h>

int main() {
    int (*collatz_conjecture)(int input);
    int (*test_collatz_conjecture)(int input, int max_iter);

    void* dll_handle = dlopen("collatz/build/libcollatz_shared.so", RTLD_LAZY);
    if (!dll_handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }

    collatz_conjecture = dlsym(dll_handle, "collatz_conjecture");
    test_collatz_conjecture = dlsym(dll_handle, "test_collatz_conjecture");

    if (!collatz_conjecture) {
        fprintf(stderr, "Error loading 'collatz_conjecture': %s\n", dlerror());
        dlclose(dll_handle);
        return 1;
    }
    if (!test_collatz_conjecture) {
        fprintf(stderr, "Error loading 'test_collatz_conjecture': %s\n", dlerror());
        dlclose(dll_handle);
        return 1;
    }

    printf("%d\n", collatz_conjecture(10));
    printf("%d\n", test_collatz_conjecture(10, 100));

    dlclose(dll_handle);
    return 0;
}