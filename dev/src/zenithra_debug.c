#include"zenithra_debug.h"
#include<time.h>
#include<stdio.h>

void zenithra_log_msg(const char* message){
    int fd = open("./dev/zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);
    char *t_buffer = zenithra_get_time();
    write(fd, t_buffer, strlen(t_buffer));
    free(t_buffer);
    write(fd, message, strlen(message));
    write(fd, "\n", strlen("\n"));
    close(fd);
}

void zenithra_log_err(const char* file_name, int line, const char* error_message){
    int fd = open("./dev/zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);
    char *t_buffer = zenithra_get_time();
    write(fd, t_buffer, strlen(t_buffer));
    free(t_buffer);
    write(fd, file_name, strlen(file_name));
    write(fd, " ", strlen(" "));
    char buffer[255];
    sprintf(buffer, "%d", line);
    write(fd, " ", strlen(" "));
    write(fd, buffer, strlen(buffer));
    write(fd, error_message, strlen(error_message));
    write(fd, "\n", strlen("\n"));
    close(fd);
    SDL_ClearError();
}

char* zenithra_get_time(){
    char *buffer;
    buffer = (char*)malloc(sizeof(char) * 255);
    time_t rawtime = time(NULL);
    struct tm *time_info;
    time_info = localtime(&rawtime);
    sprintf(buffer, "%02d:%02d:%02d: ", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
    return buffer;
}