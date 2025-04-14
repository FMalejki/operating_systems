#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <string.h>
#include <sys/stat.h>
#define MAX_LINE 128

void reverse_line(char* text){
    int lenght = strlen(text);
    if( lenght > 0 && text[lenght-1] == '\n')
        lenght--;
    for(int i = 0; i < lenght/2; i++){
        char tmp = text[i];
        text[i] = text[lenght-i-1];
        text[lenght-i-1] = tmp;
    }
}

int is_text_file(char* filename){
    char* extension = strrchr(filename,'.');
    if(!extension) return 0;
    if (strcmp(extension,".txt") == 0 || strcmp(extension,".log") == 0 || strcmp(extension,".csv") == 0)
        return 1;
    return 0;
}

void process_file(char* src_path, char* dest_path){
    FILE *file = fopen(src_path, "r");
    FILE *dest = fopen(dest_path, "w");
    if( file == NULL || dest == NULL){
        perror("error opening files");
    }
    char line[MAX_LINE];
    while(fgets(line,MAX_LINE,file) != NULL){
        reverse_line(line);
        fputs(line,dest);
    }
    fclose(dest);
    fclose(file);
}

void process_directory(char* source, char* exit){
    DIR *dir = opendir(source);
    if(dir == NULL){
        perror("cannot open dir");
        return;
    }
    mkdir(exit, 0755);
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(entry->d_type == DT_REG && is_text_file(entry->d_name) == 1){
            char src_path[MAX_LINE], dest_path[MAX_LINE];
            snprintf(src_path, sizeof(src_path), "%s/%s", source, entry->d_name);
            snprintf(dest_path, sizeof(dest_path), "%s/%s", exit, entry->d_name);
            process_file(src_path,dest_path);
        }
    }

}

int main(int argc, char** argv){
    if(argc != 3){
        perror("wrong number of arguments");
        return 1;
    }
    process_directory(argv[1], argv[2]);
}