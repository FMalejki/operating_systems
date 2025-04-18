#include <printf.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

double range_start = 0.0;
double range_stop = 1.0;

double f(double x){
    return 4/(x*x + 1);
}

double calculate_integral(double interval_start, double interval_stop, double (*fun)(double), double interval_width, unsigned long long number_of_intervals){
    if(interval_stop - interval_start < interval_width)
        return fun((interval_start + interval_stop)/ 2.0)*(interval_stop - interval_start);

    double sum = 0.0;
    for(unsigned long long i = 0; i < number_of_intervals; i++){
        sum += fun((interval_start + interval_width/2.0));
        interval_start += interval_width;
    }

    return sum * interval_width;
}

int main(int argc, char** argv) {
    if(argc < 3) {
        printf("Usage: %s <interval_width> <number_of_processes>\n", argv[0]);
        return -1;
    }

    double interval_width = strtod(argv[1], NULL);

    long num_of_processes = strtol(argv[2], NULL, 10);
    

    if(ceil((range_stop - range_start)/interval_width) < num_of_processes) {
        printf("To much processes needed for given interval range");
        return -1;
    }

    unsigned long long total_intervals_count = (unsigned long long)ceil((double)(range_stop - range_start)/interval_width);
    unsigned long long intervals_per_process = total_intervals_count/num_of_processes;


    double interval_start = range_start;
    double interval_stop = range_start;

    int pipes_fd[num_of_processes][2];

    for(int i = 0; i < num_of_processes; i++){
        if( range_stop < interval_start + intervals_per_process*interval_width){
            interval_stop = range_stop;
        } else {
            interval_stop = interval_start + intervals_per_process*interval_width;
        }

        pipe(pipes_fd[i]);
        pid_t pid = fork();
        if(pid == 0){
            close(pipes_fd[i][0]);
            double integral_result = calculate_integral(interval_start, interval_stop, f, interval_width, intervals_per_process);
            write(pipes_fd[i][1], &integral_result, sizeof(integral_result));

            exit(0);
        }
        close(pipes_fd[i][1]);

        interval_start = interval_stop;
    }

    double sum = 0.0;
    for(int i = 0; i < num_of_processes; i++){
        double integral_result;
        read(pipes_fd[i][0], &integral_result, sizeof(integral_result));
        sum += integral_result;
    } 

    printf("total result: %lf\n", sum);

    return 0;
}