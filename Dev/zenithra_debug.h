#ifdef _WIN32
	#include<windows.h>
	#include<io.h>
#else
	#include<unistd.h>
#endif

#include<fcntl.h>
#include<stdbool.h>
#include<SDL2/SDL_error.h>

void Zenithra_LogTime();
void Zenithra_LogMsg(const char* message);
void Zenithra_LogErr(const char* fileName, int line, const char* errorMessage);
void Zenithra_LogInit();
void Zenithra_LogClose(bool succesfull);
void Zenithra_LogMsgSafe(const char* message);