#include"zenithra_debug.h"
#include<time.h>
#include<stdio.h>

FILE *g_log_fp;

void zenithra_log_msg(const char* message){
    zenithra_log_time();
    fprintf(g_log_fp, "%s\n", message);
}

void zenithra_log_msg_safe(const char* message){
    int fd = open("./dev/zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);
    write(fd, message, strlen(message));
    close(fd);
}

void zenithra_log_err(const char* file_name, int line, const char* error_message){
    zenithra_log_time();
    fprintf(g_log_fp, "%s - in file %s on line %d\n", error_message, file_name, line);
    SDL_ClearError();
}

void zenithra_log_time(){
    time_t rawtime = time(NULL);
    struct tm *time_info;
    time_info = localtime(&rawtime);
    fprintf(g_log_fp, "%02d:%02d:%02d: ", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
}

void zenithra_log_init(){
    g_log_fp = fopen("./dev/zenithra_log.txt", "w");
    zenithra_log_msg("Zenithra engine started");
    fclose(g_log_fp);
    g_log_fp = fopen("./dev/zenithra_log.txt", "a");
}

void zenithra_log_close(bool successful){
    if(successful == true){
        zenithra_log_msg("Zenithra exited successfully");
        fclose(g_log_fp);
    }else{
        zenithra_log_msg("Zenithra exited unexpectedly");
        fclose(g_log_fp);
    }
}