#include <printf.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

double start = 0.0;
double end = 1.0;

double f(double x){
    return 4.0/(x*x + 1.0);
}

double calculate_integral(double range_start, double range_end, double (*fun)(double), double width, unsigned long long number_of_intervals){
    if(range_end - range_start < width)
        return fun((width + range_end)/ 2.0)*(range_end - range_start);

    double sum = 0.0;
    for(unsigned long long i = 0; i < number_of_intervals; i++){
        sum += fun((range_start + width/2.0));
        range_start += width;
    }

    return sum * width;
}

int main(int argc, char** argv){
    if(argc != 3){
        perror("error");
        return 1;
    }
    double width = strtod(argv[1], NULL);
    long procnum = strtol(argv[2], NULL, 10);

    if(ceil((end - start)/width) < procnum) {
        printf("To much processes needed for given interval range");
        return -1;
    }

    unsigned long long total_intervals_count = (unsigned long long)ceil((double)(end - start)/width);
    unsigned long long intervals_per_process = total_intervals_count/procnum;

    double interval_start = start;
    double interval_stop = end;

    int pipes_fd[procnum][2];

    for(int i = 0; i < procnum; i++){
        if( end < interval_start + intervals_per_process*width){
            interval_stop = end;
        } else {
            interval_stop = interval_start + intervals_per_process*width;
        }
        pipe(pipes_fd[i]);
        pid_t pid = fork();
        if(pid == 0){
            close(pipes_fd[i][0]);
            double integral_result = calculate_integral(interval_start, interval_stop, f, width, intervals_per_process);
            write(pipes_fd[i][1], &integral_result, sizeof(integral_result));
            exit(0);
        }
        close(pipes_fd[i][1]);
        interval_start = interval_stop;

    }

    double sum = 0.0;
    for(int i = 0; i < procnum; i++){
        double integral_results;
        read(pipes_fd[i][0], &integral_results, sizeof(integral_results));
        sum += integral_results;
    }

    printf("total result: %lf\n", sum);
    return 0;
}