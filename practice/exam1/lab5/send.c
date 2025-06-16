#include <sys/fcntl.h>

int main(int argc, char** argv){
    mkfifo("send", 0666);
    mkfifo("recieve", 0666);
    int send = open("send", O_WRONLY);
    float width = atof(argv[1]);
    int number = atoi(argv[2]);
    write(send, &width, sizeof(float));
    write(send, &number, sizeof(int));
    close(send);

    int recieve = open("recieve", O_RDONLY);
    float result;
    read(recieve, &result, sizeof(float));

    printf("Result: %f", result);
    return 0;

}