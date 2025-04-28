#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
    double a, b;
    printf("Integral range: (a,b) \n");
    scanf("%lf %lf", &a, &b);

    int fd_1 = open("pipe1", O_WRONLY);
    write(fd_1, &a, sizeof(double));
    write(fd_1, &b, sizeof(double));
    close(fd_1);

    int fd_2 = open("pipe2", O_RDONLY);
    double result;
    read(fd_2, &result, sizeof(double));
    close(fd_2);

    printf("Wynik: %f\n",result);
    return 0;
}