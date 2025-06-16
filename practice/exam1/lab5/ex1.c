#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/fcntl.h>

float fun(float x){
    return 4.0f/(x*x + 1.0f);
}


float process(float start, float end, float width){
    float field = 0;
    for(float i = start; i <= end; i += width){
        field += (width) * fun(i);
    }
    return field;
}

int main(int argc, char** argv){
    float START = 0.0;
    float END = 1.0;
    if( argc != 3){
        return 1;
    }
    //int num_of_processes = atoi(argv[2]);
    //float width = atof(argv[1]);
    int send = open("send", O_WRONLY);
    int recieve = open("recieve", O_RDONLY);
    float width;
    int num_of_processes;
    read(send, &width, sizeof(float));
    read(send, &num_of_processes, sizeof(int));
    
    int processes[num_of_processes][2];
    float distance = (END - START) / num_of_processes;
    float start = START;
    float end = start + distance;
    for(int i = 0; i < num_of_processes; i++){
        pipe(processes[i]);
        pid_t pid = fork();
        if(pid == 0){
            close(processes[i][0]);
            float result = process(start, end, width);
            write(processes[i][1], &result, sizeof(result));
            exit(0);
        } else {
            close(processes[i][1]);
            start += distance;
            end += distance;
            if( end > END) end = END;
        }
    }
    float results = 0;
    for(int i = 0; i < num_of_processes; i++){
        float temp_res;
        read(processes[i][0], &temp_res, sizeof(float));
        results += temp_res;
        wait(NULL);
    }
    write(recieve, &results, sizeof(float));

}
