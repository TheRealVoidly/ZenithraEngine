#include"zenithra_core.h"
#include<signal.h>

struct InEngineData* zenithra_init(int x, int y){
    signal(SIGSEGV, zenithra_signal_catch);
    
    struct InEngineData *engine_data_str = (struct InEngineData*)malloc(sizeof(*engine_data_str));
    engine_data_str->SDL = (struct SDLEngineData*)malloc(sizeof(*engine_data_str->SDL));
    engine_data_str->GL = (struct GLEngineData*)malloc(sizeof(*engine_data_str->GL));
    engine_data_str->MOVE = (struct MovementEngineData*)malloc(sizeof(*engine_data_str->MOVE));
    engine_data_str->KEYS = (struct KeysEngineData*)malloc(sizeof(*engine_data_str->KEYS));

    engine_data_str->obj_num = 0;
    engine_data_str->SDL->focus_lost = false;

    zenithra_log_init();
    zenithra_init_movement_vals(engine_data_str);
    zenithra_init_keys(engine_data_str);

    engine_data_str->window_x = x;
    engine_data_str->window_y = y;

    DEV_CONSOLE_CREATE;

    if(!zenithra_initialize_sdl(engine_data_str)){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "Failed to initialize SDL");
    }

    if(!zenithra_initialize_opengl(engine_data_str)){
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "Failed to initialize OpenGL");
    }else{
        zenithra_log_msg("OpenGL initialized successfuly");
    }

    zenithra_log_msg("Zenithra initialized successfuly");

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
    zenithra_log_close(true);
    exit(1);
}

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

#ifdef _WIN32
void zenithra_signal_catch(int n){
    switch(n){
    case SIGSEGV:
        zenithra_log_msg_safe("SIGSEGV");
        write(STDOUT_FILENO, "SIGSEGV\n", sizeof("SIGSEGV\n"));
        _exit(1);
        break;
    }
}
#else
void zenithra_signal_catch(int n){
    void *buffer[10];
    char **callstack;
    int frames, i;

    frames = backtrace(buffer, 10);
    callstack = backtrace_symbols(buffer, frames);

    for(i = 0; i < frames; i++){
        zenithra_log_msg_safe(callstack[i]);
        zenithra_log_msg_safe("\n");
    }

    switch(n){
    case SIGSEGV:
        zenithra_log_msg_safe("SIGSEGV");
        write(STDOUT_FILENO, "SIGSEGV\n", sizeof("SIGSEGV\n"));
        _exit(1);
        break;
    }
}
#endif

void zenithra_init_keys(struct InEngineData *engine_data_str){
    engine_data_str->KEYS->escape = false;
    engine_data_str->KEYS->r_shift = false;
}