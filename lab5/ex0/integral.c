#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <math.h>

double start = 0.0;
double end = 1.0;

double f(double x) {
    return 4.0 / (x * x + 1.0);
}

double calculate_integral(double range_start, double range_end, double (*fun)(double), double width) {
    double sum = 0.0;
    double x = range_start + width / 2.0;
    while (x < range_end) {
        sum += fun(x);
        x += width;
    }
    return sum * width;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Bad Usage");
        return 1;
    }

    double width = strtod(argv[1], NULL);
    int max_procnum = (int)strtol(argv[2], NULL, 10);

    if (width <= 0 || max_procnum <= 0) {
        printf("Invalid input parameters.\n");
        return 1;
    }

    unsigned long long total_intervals_count = (unsigned long long)ceil((end - start) / width);

    printf("width = %lf\n", width);
    printf("total intervals = %llu\n", total_intervals_count);

    for (int procnum = 1; procnum <= max_procnum; procnum++) {
        if (procnum > total_intervals_count) {
            printf("Skippeing procnum = %d, too many processes.\n", procnum);
            continue;
        }

        int pipes_fd[procnum][2];
        double interval_start = start;
        double interval_stop = end;
        unsigned long long intervals_per_process = total_intervals_count / procnum;
        unsigned long long extra_intervals = total_intervals_count % procnum;

        struct timeval t_start, t_end;
        gettimeofday(&t_start, NULL);

        for (int i = 0; i < procnum; i++) {
            unsigned long long intervals_for_this_process = intervals_per_process;
            if (i < extra_intervals) {
                intervals_for_this_process += 1;
            }

            double process_interval_start = interval_start;
            double process_interval_end = interval_start + intervals_for_this_process * width;
            if (process_interval_end > end) {
                process_interval_end = end;
            }

            if (pipe(pipes_fd[i]) == -1) {
                perror("pipe");
                exit(1);
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(1);
            }

            if (pid == 0) {
                close(pipes_fd[i][0]);
                double integral_result = calculate_integral(process_interval_start, process_interval_end, f, width);
                write(pipes_fd[i][1], &integral_result, sizeof(integral_result));
                close(pipes_fd[i][1]);
                exit(0);
            }
            close(pipes_fd[i][1]);
            interval_start = process_interval_end;
        }

        double sum = 0.0;
        for (int i = 0; i < procnum; i++) {
            double integral_result;
            read(pipes_fd[i][0], &integral_result, sizeof(integral_result));
            sum += integral_result;
            close(pipes_fd[i][0]);
        }

        for (int i = 0; i < procnum; i++) {
            wait(NULL);
        }

        gettimeofday(&t_end, NULL);

        double elapsed_time = (t_end.tv_sec - t_start.tv_sec) * 1000.0;
        elapsed_time += (t_end.tv_usec - t_start.tv_usec) / 1000.0; 

        printf("Processes: %d\tResult: %.15lf\tTime: %.3lf ms\n", procnum, sum, elapsed_time);
    }

    return 0;
}
