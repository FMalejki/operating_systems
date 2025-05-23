#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

//thread struct 
typedef struct {
    int thread_id;
    int num_threads;
    double width;
    double *outputs; //shared space ptr
} ThreadData;

double func(double x) {
    return 4.0 / (x * x + 1);
}

void* calculate_integral(void* arg) {
    //arg - ptr to thread data
    ThreadData* data = (ThreadData*) arg;
    int tid = data->thread_id;
    int k = data->num_threads;
    double width = data->width;
    double local_sum = 0.0;

    int total_steps = (int)(1.0 / width); //num of steps
    int steps_per_thread = total_steps / k;
    int start = tid * steps_per_thread;// tid as 0, 1,2 ... 
    int end = (tid == k - 1) ? total_steps : start + steps_per_thread;

    //calculating
    for (int i = start; i < end; i++) {
        double x = i * width;
        local_sum += func(x) * width;
    }

    data->outputs[tid] = local_sum;
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "bad Usage!!!");
        return 1;
    }

    double width = atof(argv[1]);
    int num_threads = atoi(argv[2]);

    pthread_t threads[num_threads];
    ThreadData data[num_threads];
    double* outputs = malloc(num_threads * sizeof(double));

    // thread init
    for (int i = 0; i < num_threads; i++) {
        data[i].thread_id = i;
        data[i].num_threads = num_threads;
        data[i].width = width;
        data[i].outputs = outputs;
        pthread_create(&threads[i], NULL, calculate_integral, &data[i]);
    }

    // wait to end threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    //adding
    double total = 0.0;
    for (int i = 0; i < num_threads; i++) {
        total += outputs[i];
    }

    printf("Wartość przybliżona całki: %.10f\n", total);

    free(outputs);
    return 0;
}
