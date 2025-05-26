#ifdef _WIN32
	#include<windows.h>
	#include<io.h>
#else
	#include<unistd.h>
#endif

#include<fcntl.h>
#include<stdbool.h>
#include<SDL2/SDL_error.h>

char* zenithra_get_time();
void zenithra_log_msg(const char* message);
void zenithra_log_err(const char* file_name, int line, const char* error_message);