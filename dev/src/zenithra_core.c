#include"zenithra_core.h"
#include<signal.h>

bool DEV_MODE = false;

/**
 * Initializes the engine.
 * Shall be called only once at startup
 *
 * Allocates all in-engine structures (SDL, movement, keys..) and sets basic initial values.
 * Also sets up signal handlers, logging, and developer console (if enabled).
 *
 * @param x the desired window width.
 * @param y the desired window height.
 * @return pointer to the fully-initialized InEngineData structure.
 *
 * @note If initialization fails at any point, zenithra_critical_error_occured is called, and the program will terminate.
**/

struct InEngineData* zenithra_init(int X, int Y){
    signal(SIGSEGV, zenithra_signal_handle);
    signal(SIGINT, zenithra_signal_handle);
    signal(SIGTERM, zenithra_signal_handle);

    zenithra_log_init();
    
    struct InEngineData *engine_data_str = malloc(sizeof *engine_data_str);
    if(!engine_data_str){
        zenithra_critical_error_occured(NULL, __FILE__, __LINE__, "engine_data_str memmory alloc failed");
    }

    engine_data_str->mem_usage = 0;
    engine_data_str->old_mem_usage = 0;

    engine_data_str->SDL = zenithra_malloc(engine_data_str, sizeof *engine_data_str->SDL);
    if(!engine_data_str->SDL){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "engine_data_str->SDL memmory alloc failed");
    }

    engine_data_str->MOVE = zenithra_malloc(engine_data_str, sizeof *engine_data_str->MOVE);
    if(!engine_data_str->MOVE){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "engine_data_str->MOVE memmory alloc failed");
    }

    engine_data_str->KEYS = zenithra_malloc(engine_data_str, sizeof *engine_data_str->KEYS);
    if(!engine_data_str->KEYS){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "engine_data_str->KEYS memmory alloc failed");
    }

    engine_data_str->INTERPRETER = zenithra_malloc(engine_data_str, sizeof *engine_data_str->INTERPRETER);
    if(!engine_data_str->INTERPRETER){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "engine_data_str->INTERPRETER memmory alloc failed");
    }

    engine_data_str->focus_lost = false; //Window starts in focus

    zenithra_log_msg("Zenithra engine started");
    zenithra_init_movement_vals(engine_data_str);
    zenithra_init_keys(engine_data_str);

    engine_data_str->window_X = X;
    engine_data_str->window_Y = Y;

    DEV_CONSOLE_CREATE; //Creates developer console if DEV_MODE is true. Only on Windows

    if(!zenithra_initialize_sdl(engine_data_str)){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "Failed to initialize SDL");
    }else{
        zenithra_log_msg("SDL initialized successfully");
    }

    zenithra_create_point_buffer(engine_data_str);

    engine_data_str->obj_number = 0;

    zenithra_log_msg("Zenithra initialized successfully");

    return engine_data_str;
}

bool zenithra_initialize_sdl(struct InEngineData *engine_data_str){
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    engine_data_str->SDL->window = SDL_CreateWindow("Zenithra Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, engine_data_str->window_X, engine_data_str->window_Y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(engine_data_str->SDL->window == NULL){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

    engine_data_str->SDL->renderer = SDL_CreateRenderer(engine_data_str->SDL->window, -1, 0);
    if(engine_data_str->SDL->renderer == NULL){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }
    SDL_GetRendererOutputSize(engine_data_str->SDL->renderer, &engine_data_str->renderer_X, &engine_data_str->renderer_Y);

    SDL_GLContext context = SDL_GL_CreateContext(engine_data_str->SDL->window);
    if(!context){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

    #ifdef __linux__
    zenithra_disable_bypass_compositor(engine_data_str->SDL->window);
    #endif

    SDL_SetRelativeMouseMode(SDL_TRUE);

    return true;
}

/**
 * Ends the engine
 * Shall be called only at the end of program as this terminates the process
 * 
 * Frees and destroys everything allocated
 * 
 * @param engine_data_str = Needed to be passed to the function to free allocated in-engine struct
**/

void zenithra_destroy(struct InEngineData *engine_data_str){
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_SetWindowGrab(engine_data_str->SDL->window, SDL_FALSE);

    SDL_DestroyRenderer(engine_data_str->SDL->renderer);
    SDL_DestroyWindow(engine_data_str->SDL->window);

    SDL_Quit();

    zenithra_destroy_object(engine_data_str, -1);

    zenithra_destroy_point_buffer(engine_data_str);

    free(engine_data_str->INTERPRETER->command);
    zenithra_interpreter_free_variable_list(engine_data_str, (void*)&engine_data_str->INTERPRETER->iv);
    zenithra_free(engine_data_str, (void**)&engine_data_str->MOVE, sizeof *engine_data_str->MOVE);
    zenithra_free(engine_data_str, (void**)&engine_data_str->SDL, sizeof *engine_data_str->SDL);
    zenithra_free(engine_data_str, (void**)&engine_data_str->INTERPRETER, sizeof *engine_data_str->INTERPRETER);
    zenithra_free(engine_data_str, (void**)&engine_data_str->KEYS, sizeof *engine_data_str->KEYS);

    zenithra_check_and_display_memory_change(engine_data_str);

    zenithra_free(engine_data_str, (void**)&engine_data_str, sizeof *engine_data_str);

    zenithra_log_msg("Zenithra exited successfully");
}

/**
 * A critical error logger
 * 
 * @param engnine_data_str
 * @param file_name name of the file where the critical error occured
 * @param line line where the critical error occured
 * @param error error message
**/

void zenithra_critical_error_occured(struct InEngineData *engine_data_str, char* file_name, int line, const char* error){
    fprintf(stdout, "%s\n", error);
    zenithra_log_err(file_name, line, error);
    zenithra_log_msg("Error Is Critcal - Exiting Now");
    zenithra_destroy(engine_data_str);
    exit(1);
}

/**
 * Safe way to free stuff
 * Frees and sets pointer to NULL
 * 
 * @param pp double pointer of the thing to be freed
**/

void zenithra_free(struct InEngineData *engine_data_str, void **pp, size_t size){
    if(!*pp){
        zenithra_log_msg("Attempt to free null pointer");
        return;
    }

    engine_data_str->mem_usage -= size;

    void *p;
    p = *pp;
    free(p);
    *pp = NULL;
    p = NULL;
}

/**
 * Signal handler function
 * @param sig signal
**/

void zenithra_signal_handle(int sig){
    char *t_buffer;
    int fd;

    #ifdef __linux__
    void *buffer[15];
    char **callstack;
    int frames;
    int i;
    #endif

    switch(sig){
    case SIGSEGV:

        #ifdef __linux__
        frames = backtrace(buffer, 15);
        callstack = backtrace_symbols(buffer, frames);

        fd = open("./dev/zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);

        for(i = frames - 1; i > 0; i--){
            write(fd, callstack[i], strlen(callstack[i]));
            write(fd, "\n", strlen("\n"));
        }
        #else
        fd = open("./dev/zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);
        #endif

        t_buffer = zenithra_get_time();
        write(fd, t_buffer, strlen(t_buffer));
        free(t_buffer);
        write(fd, "SIGSEGV", strlen("SIGSEGV"));
        write(STDOUT_FILENO, "SIGSEGV\n", sizeof("SIGSEGV\n"));
        close(fd);
        _exit(1);
        break;

    case SIGINT:
    case SIGTERM:
        program_should_quit = true;
        break;
    }
}

void zenithra_init_keys(struct InEngineData *engine_data_str){
    engine_data_str->KEYS->escape = false;
}

#ifdef __linux__

/**
 * Function defined only for Linux
 * Handles real-time console input
**/

int _kbhit(){
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds); // File descriptor 0 = stdin
    return select(1, &fds, NULL, NULL, &tv);
}

/**
 * Function defined only for Linux
 * Disables compositor bypass if one is active
**/

void zenithra_disable_bypass_compositor(SDL_Window *window){
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if(!SDL_GetWindowWMInfo(window, &wmInfo)){
        SDL_Log("SDL_GetWindowWMInfo failed: %s", SDL_GetError());
        zenithra_log_err(__FILE__, __LINE__, "Failed to disable compositor bypass");
        return;
    }

    Display *display = wmInfo.info.x11.display;
    Window xwindow = wmInfo.info.x11.window;

    Atom bypass = XInternAtom(display, "_NET_WM_BYPASS_COMPOSITOR", False);
    if(bypass){
        unsigned long value = 0;
        XChangeProperty(display, xwindow, bypass, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&value, 1);
        XFlush(display);
    }
}

#endif

/**
 * Should be used for dynamic allocation instead of malloc for memory logging purposes
**/

void *zenithra_malloc(struct InEngineData *engine_data_str, size_t size){
    void *ret_p;
    ret_p = malloc(size);
    if(!ret_p){
        return NULL;
    }
    engine_data_str->mem_usage += size;
    return ret_p;
}

/**
 * Should be used for dynamic reallocation instead of realloc for memory logging purposes
**/

void *zenithra_realloc(struct InEngineData *engine_data_str, void *p, size_t new_size, size_t old_size){
    void *ret_p;
    ret_p = realloc(p, new_size);
    if(!ret_p){
        return NULL;
    }
    engine_data_str->mem_usage -= old_size;
    engine_data_str->mem_usage += new_size;
    return ret_p;
}

/**
 * Checks if the memory usage changed since last check and displays the new value in MB
**/

void zenithra_check_and_display_memory_change(struct InEngineData *engine_data_str){
    if(engine_data_str->mem_usage != engine_data_str->old_mem_usage){
        if(DEV_MODE){
            printf("%f\n", (float)((float)engine_data_str->mem_usage / 1048576.0));
        }
        engine_data_str->old_mem_usage = engine_data_str->mem_usage;
    }
}