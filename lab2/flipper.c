#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_LINE 128

int is_text_file(const char *filename){
    const char *ext = strrchr(filename,'.');
    if (!ext) return 0;
    if (strcmp(ext, ".txt") == 0 || strcmp(ext,".log") == 0 || strcmp(ext, ".csv") == 0 ){
        return 1;
    }
    return 0;
}

void reverse_line(char *line) {
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') len--;
    for (int i = 0; i < len / 2; i++) {
        char temp = line[i];
        line[i] = line[len - i - 1];
        line[len - i - 1] = temp;
    }
}

void process_file(const char *src_path, const char *dest_path) {
    if(!is_text_file(src_path)){
        return;
    }

    FILE *src = fopen(src_path, "r");
    if (!src) {
        perror("Error opening source path");
        return;
    }

    FILE *dest = fopen(dest_path, "w");
    if (!dest) {
        perror("Error opening destination path");
        fclose(src);
        return;
    }

    char line[MAX_LINE];
    //reading line by line
    while (fgets(line, MAX_LINE, src)) {
        reverse_line(line);
        //saving
        fputs(line, dest);
    }

    fclose(src);
    fclose(dest);
}

void process_directory(const char *src_dir, const char *dest_dir) {
    DIR *dir = opendir(src_dir);
    if (!dir) {
        perror("Error opening dir");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;

    //rwx wx wx
    mkdir(dest_dir, 0755);
    //if exists err but we dont check it

    //reading to the end
    while ((entry = readdir(dir)) != NULL) {
        //checking if regular and then if text file (log, txt, csv)
        if (entry->d_type == DT_REG && is_text_file(entry->d_name)) {
            char src_path[MAX_LINE], dest_path[MAX_LINE];
            
            snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);
            snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_dir, entry->d_name);

            process_file(src_path, dest_path);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    process_directory(argv[1], argv[2]);
    return 0;
}