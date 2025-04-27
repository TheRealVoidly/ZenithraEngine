#include"zenithra_debug.h"
#include<time.h>
#include<stdio.h>

FILE *logFp;

void Zenithra_LogMsg(const char* message){
    Zenithra_LogTime();
    fprintf(logFp, "%s\n", message);
}

void Zenithra_LogMsgSafe(const char* message){
    int fd = open("./zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);
    write(fd, message, strlen(message));
    close(fd);
}

void Zenithra_LogErr(const char* fileName, int line, const char* errorMessage){
    Zenithra_LogTime();
    fprintf(logFp, "%s - in file %s on line %d\n", errorMessage, fileName, line);
    SDL_ClearError();
}

void Zenithra_LogTime(){
    time_t rawtime = time(NULL);
    struct tm *timeinfo;
    timeinfo = localtime(&rawtime);
    fprintf(logFp, "%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void Zenithra_LogInit(){
    logFp = fopen("./zenithra_log.txt", "w");
    Zenithra_LogMsg("Zenithra engine started");
    fclose(logFp);
    logFp = fopen("./zenithra_log.txt", "a");
}

void Zenithra_LogClose(bool succesfull){
    if(succesfull == true){
        Zenithra_LogMsg("Zenithra exited succesfully");
        fclose(logFp);
    }else{
        Zenithra_LogMsg("Zenithra exited unexpectedly");
        fclose(logFp);
    }
}