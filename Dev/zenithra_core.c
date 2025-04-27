#include"zenithra_core.h"
#include<signal.h>

struct in_engine_data* Zenithra_Init(int x, int y){
    signal(SIGSEGV, Zenithra_SignalCatch);
    
    struct in_engine_data *engineDataStr = (struct in_engine_data*)malloc(sizeof(*engineDataStr));
    engineDataStr->SDL = (struct sdl_engine_data*)malloc(sizeof(*engineDataStr->SDL));
    engineDataStr->GL = (struct gl_engine_data*)malloc(sizeof(*engineDataStr->GL));
    engineDataStr->MOVE = (struct movement_engine_data*)malloc(sizeof(*engineDataStr->MOVE));
    engineDataStr->KEYS = (struct keys_engine_data*)malloc(sizeof(*engineDataStr->KEYS));

    engineDataStr->objNum = 0;
    engineDataStr->SDL->focusLost = false;

    Zenithra_LogInit();
    Zenithra_InitMovementVals(engineDataStr);
    Zenithra_InitKeys(engineDataStr);

    engineDataStr->window_x = x;
    engineDataStr->window_y = y;

    DEV_CONSOLE_CREATE;

    if(!Zenithra_InitializeSDL(engineDataStr)){
        Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, "Failed to initialize SDL");
    }

    if(!Zenithra_InitializeOpenGL(engineDataStr)){
        Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, "Failed to initialize OpenGL");
    }else{
        Zenithra_LogMsg("OpenGL initialized successfuly");
    }

    Zenithra_LogMsg("Zenithra initialized successfuly");

    return engineDataStr;
}

bool Zenithra_InitializeSDL(struct in_engine_data *engineDataStr){
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, SDL_GetError());
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

    engineDataStr->SDL->window = SDL_CreateWindow("Zenithra Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, engineDataStr->window_x, engineDataStr->window_y, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(engineDataStr->SDL->window == NULL){
        Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, SDL_GetError());
    }

    engineDataStr->SDL->renderer = SDL_CreateRenderer(engineDataStr->SDL->window, -1, 0);
    if(engineDataStr->SDL->renderer == NULL){
        Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, SDL_GetError());
    }

    SDL_GLContext context = SDL_GL_CreateContext(engineDataStr->SDL->window);
    if(!context){
        Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, SDL_GetError());
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK){
        Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, (const char*)glewGetErrorString(glewError));
    }
    if(SDL_GL_SetSwapInterval(1) < 0){
        Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, SDL_GetError());
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    return true;
}

void Zenithra_Destroy(struct in_engine_data *engineDataStr){
    glDeleteProgram(engineDataStr->GL->programID);
    glDeleteVertexArrays(1, &engineDataStr->GL->vertexArrayID);
    SDL_DestroyRenderer(engineDataStr->SDL->renderer);
    SDL_DestroyWindow(engineDataStr->SDL->window);

    Zenithra_Free((void*)&engineDataStr->MOVE);
    Zenithra_Free((void*)&engineDataStr->SDL);
    Zenithra_Free((void*)&engineDataStr->GL);
    Zenithra_Free((void*)&engineDataStr);

    SDL_Quit();
    Zenithra_LogClose(true);
    exit(1);
}

void Zenithra_CriticalErrorOccured(struct in_engine_data *engineDataStr, char* fileName, int line, const char* error){
    fprintf(stdout, "%s\n", error);
    Zenithra_LogErr(fileName, line, error);
    Zenithra_LogMsg("Error Is Critcal - Exiting Now");
    Zenithra_Destroy(engineDataStr);
    exit(1);
}

bool Zenithra_InitializeOpenGL(struct in_engine_data *engineDataStr){
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);

    glGenVertexArrays(1, &engineDataStr->GL->vertexArrayID);
    glBindVertexArray(engineDataStr->GL->vertexArrayID);

    engineDataStr->GL->programID = Zenithra_LoadShaders(engineDataStr);
    engineDataStr->GL->matrixID = glGetUniformLocation(engineDataStr->GL->programID, "mvp");

    return true;
}

void Zenithra_Free(void **pp){
    if(!*pp){
        Zenithra_LogMsg("Attempt to free null pointer");
        return;
    }

    void *p;
    p = *pp;
    free(p);
    *pp = NULL;
    p = NULL;
}

void Zenithra_FreeList(void **head){
    if(!*head){
        Zenithra_LogMsg("Attempt to free null list");
        return;
    }

    struct temp_list{
        struct temp_list *next;
        char *temp_arr_fill;
    };

    void *temp;
    while(((void*)(((struct temp_list*)*head)->next))){
        temp = *head;
        *head = ((void*)(((struct temp_list*)*head)->next));
        Zenithra_Free(&temp);
        if(!((void*)(((struct temp_list*)*head)->next))){
            Zenithra_Free(head);
            break;
        }
    }
}

#ifdef _WIN32
void Zenithra_SignalCatch(int n){
    switch(n){
    case SIGSEGV:
        Zenithra_LogMsgSafe("SIGSEGV");
        write(STDOUT_FILENO, "SIGSEGV\n", sizeof("SIGSEGV\n"));
        _exit(1);
        break;
    }
}
#else
void Zenithra_SignalCatch(int n){
    void *buffer[10];
    char **callstack;
    int frames, i;

    frames = backtrace(buffer, 10);
    callstack = backtrace_symbols(buffer, frames);

    for(i = 0; i < frames; i++){
        Zenithra_LogMsgSafe(callstack[i]);
        Zenithra_LogMsgSafe("\n");
    }

    switch(n){
    case SIGSEGV:
        Zenithra_LogMsgSafe("SIGSEGV");
        write(STDOUT_FILENO, "SIGSEGV\n", sizeof("SIGSEGV\n"));
        _exit(1);
        break;
    }
}
#endif

void* Zenithra_CreateNode(void **node, bool isHead, int structTypeSize){
    struct temp_list{
        struct temp_list *next;
        char temp_arr_fill[structTypeSize-8];
    };
    void *newNode;

    if(isHead == true){
        newNode = (void*)malloc(sizeof(struct temp_list));
        ((struct temp_list*)newNode)->next = NULL;
    }else{
        if(((void*)(((struct temp_list*)*node)->next))){
            Zenithra_LogMsg("Attempt to create node with non-NULL pointer");
            return newNode;
        }
        ((struct temp_list*)*node)->next = (void*)malloc(sizeof(struct temp_list));
        newNode = ((void*)(((struct temp_list*)*node)->next));
        ((struct temp_list*)newNode)->next = NULL;
    }
    
    return (struct temp_list*)newNode;
}

void Zenithra_InitKeys(struct in_engine_data *engineDataStr){
    engineDataStr->KEYS->escape = false;
    engineDataStr->KEYS->rShift = false;
}