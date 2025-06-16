#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    int thread_id;
    int num_threads;
    double width;
    double *outputs;
} ThreadData;

double func(double x) {
    return 4.0 / (x * x + 1);
}

void* calculate_integral(void* arg){
    ThreadData* data = (ThreadData*) arg;
    int tid = data->thread_id;
    int num_of_threads = data->num_threads;
    double width = data->width;
    double sum = 0.0;

    int total_steps = (int)(1.0 / width);
    int steps_per_thread = total_steps / num_of_threads;

    int start = tid * steps_per_thread; 
    int end = (tid == num_of_threads - 1) ? total_steps : start + steps_per_thread;

    for(int i = start; i < end; i++){
        double x = i * width;
        sum += func(x) * width;
    }
    data->outputs[tid] = sum;
    exit(0);
}

int main(int argc, char** argv){
    if(argc != 3){
        printf("bad usage");
        exit(1);
    }

    double width = atof(argv[1]);
    int num_of_threads = atoi(argv[2]);

    pthread_t threads[num_of_threads];
    ThreadData data[num_of_threads];
    double *outputs = malloc(num_of_threads * sizeof(double));

    for(int i = 0; i < num_of_threads; i++){
        data[i].thread_id = i;
        data[i].num_threads = num_of_threads;
        data[i].width = width;
        data[i].outputs = outputs;
        pthread_create(&threads[i], NULL, calculate_integral, &data[i]);
    }

    for(int i = 0; i < num_of_threads; i++){
        pthread_join(threads[i], NULL);
    }

    double total = 0.0;
    for (int i = 0; i < num_of_threads; i++) {
        total += outputs[i];
    }

    printf("Wartość przybliżona całki: %.10f\n", total);

    free(outputs);

    return 0;

}