#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <SDL2/SDL_error.h>
#include <fcntl.h>
#include <stdbool.h>

char *zenithra_get_time();
void zenithra_log_msg(const char *message);
void zenithra_log_err(const char *file_name, int line, const char *error_message);
void zenithra_log_init();