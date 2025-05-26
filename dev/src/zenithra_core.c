#include"zenithra_core.h"
#include<signal.h>

/**
 * Initializes the engine.
 * Shall be called only once at startup
 *
 * Allocates all in-engine structures (SDL, OpenGL, movement, keys) and sets basic initial values.
 * Also sets up signal handlers, logging, and developer console (if enabled).
 *
 * @param x = The desired window width.
 * @param y = The desired window height.
 * @return A pointer to the fully-initialized InEngineData structure.
 * 
 * Counter part is zenithra_destroy which frees everything and stops the engine
 *
 * @note If initialization fails at any point, zenithra_critical_error_occured is called, and the program will terminate.
**/

struct InEngineData* zenithra_init(int x, int y){
    signal(SIGSEGV, zenithra_signal_catch);
    
    struct InEngineData *engine_data_str = (struct InEngineData*)malloc(sizeof(*engine_data_str));
    engine_data_str->SDL = (struct SDLEngineData*)malloc(sizeof(*engine_data_str->SDL));
    engine_data_str->GL = (struct GLEngineData*)malloc(sizeof(*engine_data_str->GL));
    engine_data_str->MOVE = (struct MovementEngineData*)malloc(sizeof(*engine_data_str->MOVE));
    engine_data_str->KEYS = (struct KeysEngineData*)malloc(sizeof(*engine_data_str->KEYS));

    engine_data_str->obj_num = 0; //Number of loaded objects is 0 at initialization
    engine_data_str->focus_lost = false; // Window starts in focus

    zenithra_log_msg("Zenithra engine started");
    zenithra_init_movement_vals(engine_data_str);
    zenithra_init_keys(engine_data_str);

    engine_data_str->window_x = x;
    engine_data_str->window_y = y;

    DEV_CONSOLE_CREATE; // Creates developer console if DEV_MODE is defined. Only on Windows

    if(!zenithra_initialize_sdl(engine_data_str)){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "Failed to initialize SDL");
    }

    if(!zenithra_initialize_opengl(engine_data_str)){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "Failed to initialize OpenGL");
    }else{
        zenithra_log_msg("OpenGL initialized successfully");
    }

    zenithra_log_msg("Zenithra initialized successfully");

    return engine_data_str;
}

bool zenithra_initialize_sdl(struct InEngineData *engine_data_str){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
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

    engine_data_str->SDL->window = SDL_CreateWindow("Zenithra Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, engine_data_str->window_x, engine_data_str->window_y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(engine_data_str->SDL->window == NULL){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

    engine_data_str->SDL->renderer = SDL_CreateRenderer(engine_data_str->SDL->window, -1, 0);
    if(engine_data_str->SDL->renderer == NULL){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

    SDL_GLContext context = SDL_GL_CreateContext(engine_data_str->SDL->window);
    if(!context){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

    glewExperimental = GL_TRUE;
    GLenum glew_error = glewInit();
    if(glew_error != GLEW_OK){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, (const char*)glewGetErrorString(glew_error));
    }
    if(SDL_GL_SetSwapInterval(1) < 0){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

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
    glDeleteProgram(engine_data_str->GL->program_id);
    glDeleteVertexArrays(1, &engine_data_str->GL->vertex_array_id);
    SDL_DestroyRenderer(engine_data_str->SDL->renderer);
    SDL_DestroyWindow(engine_data_str->SDL->window);

    zenithra_free((void*)&engine_data_str->MOVE);
    zenithra_free((void*)&engine_data_str->SDL);
    zenithra_free((void*)&engine_data_str->GL);
    zenithra_free((void*)&engine_data_str);

    SDL_Quit();
    zenithra_log_msg("Zenithra exited successfully");
    exit(1);
}

/**
 * If a critical error occures this function should be called
 * It is the safe way to terminate the program even after a critical error
 * 
 * @param engnine_data_str
 * @param file_name = Name of the file where the critical error occured
 * @param line = Line where the critical error occured
 * @param error = Error message
**/

void zenithra_critical_error_occured(struct InEngineData *engine_data_str, char* file_name, int line, const char* error){
    fprintf(stdout, "%s\n", error);
    zenithra_log_err(file_name, line, error);
    zenithra_log_msg("Error Is Critcal - Exiting Now");
    zenithra_destroy(engine_data_str);
    exit(1);
}

bool zenithra_initialize_opengl(struct InEngineData *engine_data_str){
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);

    glGenVertexArrays(1, &engine_data_str->GL->vertex_array_id);
    glBindVertexArray(engine_data_str->GL->vertex_array_id);

    engine_data_str->GL->program_id = zenithra_load_shaders(engine_data_str);
    engine_data_str->GL->matrix_id = glGetUniformLocation(engine_data_str->GL->program_id, "mvp");

    return true;
}

/**
 * Safe way to free stuff
 * Frees and sets pointer to NULL
 * 
 * @param pp = double pointer of the thing to be freed
**/

void zenithra_free(void **pp){
    if(!*pp){
        zenithra_log_msg("Attempt to free null pointer");
        return;
    }

    void *p;
    p = *pp;
    free(p);
    *pp = NULL;
    p = NULL;
}

/**
 * Signal catche functions
 * @param n = signal
**/

#ifdef _WIN32
void zenithra_signal_catch(int n){
    switch(n){
    case SIGSEGV:
        int fd = open("./dev/zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);
        char *t_buffer = zenithra_get_time();
        write(fd, t_buffer, strlen(t_buffer));
        free(t_buffer);
        write(fd, "SIGSEGV", strlen("SIGSEGV"));
        write(STDOUT_FILENO, "SIGSEGV\n", sizeof("SIGSEGV\n"));
        close(fd);
        _exit(1);
        break;
    }
}
#else
void zenithra_signal_catch(int n){
    void *buffer[15];
    char **callstack;

    int frames = backtrace(buffer, 15);
    callstack = backtrace_symbols(buffer, frames);

    int fd = open("./dev/zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);

    for(int i = frames - 1; i > 0; i--){
        write(fd, callstack[i], strlen(callstack[i]));
        write(fd, "\n", strlen("\n"));
    }

    char *t_buffer;
    switch(n){
    case SIGSEGV:
        t_buffer = zenithra_get_time();
        write(fd, t_buffer, strlen(t_buffer));
        free(t_buffer);
        write(fd, "SIGSEGV", strlen("SIGSEGV"));
        write(STDOUT_FILENO, "SIGSEGV\n", sizeof("SIGSEGV\n"));
        close(fd);
        _exit(1);
        break;
    }
}
#endif

void zenithra_init_keys(struct InEngineData *engine_data_str){
    engine_data_str->KEYS->escape = false;
    engine_data_str->KEYS->r_shift = false;
}

/**
 * Function defined only for Linux
 * Handles real-time console input
**/

#ifndef _WIN32
int _kbhit(){
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds); // file descriptor 0 = stdin
    return select(1, &fds, NULL, NULL, &tv);
}
#endif