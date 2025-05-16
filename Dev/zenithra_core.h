#ifdef _WIN32
    #include<windows.h>
    #include<stdio.h>
    #include<stdlib.h>
#else
    #include<execinfo.h>
    #include<unistd.h>
#endif
#include<SDL2/SDL.h>
#include<SDL2/SDL_render.h>
#include<SDL2/SDL_video.h>
#include<SDL2/SDL_pixels.h>
#include<SDL2/SDL_rect.h>
#include<SDL2/SDL_surface.h>

#define GLEW_STATIC

#include<GL/glew.h>
#include<SDL2/SDL_opengl.h>
#include<cglm/cglm.h>
#include"zenithra_debug.h"

#define DEV_MODE

#ifdef _WIN32
    #ifdef DEV_MODE
        #define DEV_CONSOLE_CREATE ({AllocConsole(); freopen("CONOUT$", "wb", stdout); freopen("CONIN$", "rb", stdin); freopen("CONOUT$", "wb", stderr);})
    #else
        #define DEV_CONSOLE_CREATE
    #endif
#else
    #define DEV_CONSOLE_CREATE
#endif

//-----------------------------------------------
//Core
//-----------------------------------------------

#define ZENITHRA_KEYCALLBACK 1
#define ZENITHRA_CONSOLECALLBACK 2

typedef struct sdl_engine_data{
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool focusLost;
}SDL;

typedef struct gl_engine_data{
    GLuint programID;
    GLuint matrixID;
    GLuint vertexArrayID;
}GL;

typedef struct movement_engine_data{
    vec3 position;
    vec3 directionLook;
    vec3 directionStrafe;
    vec3 directionVertical;
    float horizontalAngle;
    float verticalAngle;
    float FOV;
    float speed;
    float mouseSpeed;
    float gravity;
    int xPos, yPos;
}MOVE;

typedef struct keys_engine_data{
    bool escape;
    bool rShift;
}KEYS;

struct in_engine_data{
    MOVE *MOVE;
    SDL *SDL;
    GL *GL;
    KEYS *KEYS;

    float deltaTime;
    int window_x;
    int window_y;

    int objNum;
};

void Zenithra_SignalCatch(int n);
void Zenithra_Free(void **pp);
void Zenithra_FreeList(void **head);
void* Zenithra_CreateNode(void **node, bool isHead, int structTypeSize);
struct in_engine_data* Zenithra_Init(int x, int y);
void Zenithra_Destroy(struct in_engine_data *engineDataStr);
void Zenithra_CriticalErrorOccured(struct in_engine_data *engineDataStr, char* fileName, int line, const char* error);
bool Zenithra_InitializeOpenGL(struct in_engine_data *engineDataStr);
bool Zenithra_InitializeSDL(struct in_engine_data *engineDataStr);
void Zenithra_InitKeys(struct in_engine_data *engineDataStr);

//-----------------------------------------------
//Movement
//-----------------------------------------------

void Zenithra_InitMovementVals(struct in_engine_data *engineDataStr);
void Zenithra_CalcMouseMovement(struct in_engine_data *engineDataStr);
void Zenithra_UpdatePosition(struct in_engine_data *engineDataStr);

//-----------------------------------------------
//Events
//-----------------------------------------------

bool Zenithra_HandleEventPoll(struct in_engine_data *engineDataStr);

//-----------------------------------------------
//Graphics
//-----------------------------------------------

struct list_temp_vec3{
    struct list_temp_vec3 *next;
    long double data[3];
};

struct list_temp_vec2{
    struct list_temp_vec2 *next;
    long double data[2];
};

struct list_temp_mat3{
    struct list_temp_mat3 *next;
    long long data[3][3];
};

struct object_data{
    long long objSize;
    GLuint objNormalBuffer;
    GLuint objVertexBuffer;
    GLuint objUVBuffer;

    vec3 translationVector;

    GLfloat *vertex_buffer_data;

    long long triangles;
};

GLuint Zenithra_LoadShaders(struct in_engine_data *engineDataStr);
GLuint Zenithra_CreateTexture(const char* fileName);
struct object_data* Zenithra_LoadOBJ(struct in_engine_data *engineDataStr, const char* fileName);
void Zenithra_RenderObject(struct in_engine_data *engineDataStr, struct object_data **obj, int objNum, GLuint texID);