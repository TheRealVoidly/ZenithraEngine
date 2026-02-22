#include <stddef.h>
#ifdef _WIN32
    #include<windows.h>
    #include<stdio.h>
    #include<stdlib.h>
    #include<conio.h>
#else
    #include<X11/Xlib.h>
    #include<X11/Xatom.h>
    #include<SDL2/SDL_syswm.h>
    #include<execinfo.h>
    #include<unistd.h>
#endif
#include<SDL2/SDL.h>
#include<SDL2/SDL_render.h>
#include<SDL2/SDL_video.h>
#include<SDL2/SDL_pixels.h>
#include<SDL2/SDL_rect.h>
#include<SDL2/SDL_surface.h>
#include<SDL2/SDL_scancode.h>

#include<cglm/cglm.h>
#include"zenithra_debug.h"

//These have to be set before zenithra_init()
extern bool DEV_MODE; //Set as true if creating a console (on Windows) is desired along-side dev statistics like memory usage, default false
extern int GAME_DIMENSIONALITY; //2 = 2D, 3 = 3D, default 3

#ifdef _WIN32
    #if DEV_MODE
        #define DEV_CONSOLE_CREATE ({AllocConsole(); freopen("CONOUT$", "wb", stdout); freopen("CONIN$", "rb", stdin); freopen("CONOUT$", "wb", stderr);})
    #else
        #define DEV_CONSOLE_CREATE
    #endif
#else
    #define DEV_CONSOLE_CREATE
#endif

//-----------------------------------------------
// Graphics Structs
//-----------------------------------------------

struct TempNormCoords{
    double norm_X;
    double norm_Y;
};

typedef struct PointBuffer{
    int X;
    int Y;

    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 w;

    struct PointBuffer *next;
}POINT_BUF;

typedef struct ObjectVerticeData{
    double x, y, z;
    size_t size;
}ZBJ_VERTICE_DATA;

typedef struct ObjectFaceData{
    int f1, f2, f3;
    size_t size;
}ZBJ_FACE_DATA;

typedef struct ObjectList{
    ZBJ_FACE_DATA *face;
    ZBJ_VERTICE_DATA *vertice;
}ZBJ_LIST;

//-----------------------------------------------
// Movement Structs
//-----------------------------------------------

typedef struct MovementEngineData{
    vec3 position;
    vec3 direction_look;
    vec3 direction_strafe;
    vec3 direction_vertical;

    float horizontal_angle;
    float vertical_angle;
    float fov;
    float speed;
    float mouse_speed;
    float gravity;

    int x_pos, y_pos;
}MOVE;

//-----------------------------------------------
// Interpreter Structs
//-----------------------------------------------

struct InterpreterVariable{
    char variable_name[1024];

    float f_value;

    struct InterpreterVariable *next;
};

typedef struct ReadData{
    char *command;
    int fval;
    int offset;

    struct InterpreterVariable *iv;
}INTERPRETER;

//-----------------------------------------------
// Core Structs
//-----------------------------------------------

typedef struct SDLEngineData{
    SDL_Window *window;
    SDL_Renderer *renderer;
}SDL;

typedef struct KeysEngineData{
    bool escape;
}KEYS;

struct InEngineData{
    MOVE *MOVE;
    SDL *SDL;   
    KEYS *KEYS;
    INTERPRETER *INTERPRETER;
    POINT_BUF *POINT_BUF;
    ZBJ_LIST *ZBJ_LIST;

    int obj_number;

    int window_X;
    int window_Y;

    int renderer_X;
    int renderer_Y;

    bool focus_lost;

    int mem_usage;
    int old_mem_usage;
};

//-----------------------------------------------
// Core Funcs
//-----------------------------------------------

#ifndef WIN32 //If not on Windows define a function for reading from console input. If on windows such fuction is provided from a header
int _kbhit();
#endif

void zenithra_signal_catch(int n);
void zenithra_free(struct InEngineData *engine_data_str, void **pp, size_t size);
struct InEngineData* zenithra_init(int X, int Y);
void zenithra_destroy(struct InEngineData *engine_data_str);
void zenithra_critical_error_occured(struct InEngineData *engine_data_str, char* file_name, int line, const char* error);
bool zenithra_initialize_sdl(struct InEngineData *engine_data_str);
void zenithra_init_keys(struct InEngineData *engine_data_str);
void zenithra_disable_bypass_compositor(SDL_Window *window);
void *zenithra_malloc(struct InEngineData *engine_data_str, size_t size);
void zenithra_check_and_display_memory_change(struct InEngineData *engine_data_str);
void *zenithra_realloc(struct InEngineData *engine_data_str, void *p, size_t new_size, size_t old_size);

//-----------------------------------------------
// Movement Funcs
//-----------------------------------------------

void zenithra_init_movement_vals(struct InEngineData *engine_data_str);
void zenithra_calc_mouse_movement(struct InEngineData *engine_data_str);
void zenithra_update_position(struct InEngineData *engine_data_str);

//-----------------------------------------------
// Events Funcs
//-----------------------------------------------

bool zenithra_handle_event_poll(struct InEngineData *engine_data_str);

//-----------------------------------------------
// Graphics Funcs
//-----------------------------------------------

void zenithra_create_point_buffer(struct InEngineData *engine_data_str);
void zenithra_save_points_for_rendering(struct InEngineData *engine_data_str, double Xw, double Yw, double Zw);
void zenithra_draw(struct InEngineData *engine_data_str);
int zenithra_load_object(struct InEngineData *engine_data_str, char *file_name);
void zenithra_destroy_object(struct InEngineData *engine_data_str, int index);
struct TempNormCoords* zenithra_normalize_vertice(struct InEngineData *engine_data_str, double Xw, double Yw, double Zw);
void zenithra_destroy_point_buffer(struct InEngineData *engine_data_str);

//-----------------------------------------------
// Editor Funcs
//-----------------------------------------------

#define START_OF_OBJECT_INDEX 1

int* zenithra_object_ray_intersects_detection(float origin[3], struct InEngineData *engine_data_str);

//-----------------------------------------------
// Interpreter Funcs
//-----------------------------------------------

void zenithra_interpreter_begin(struct InEngineData *engine_data_str);
void zenithra_register_callback(struct InEngineData *engine_data_str, char *callback_name, char *callback_request);
void zenithra_interpreter_loop(struct InEngineData *engine_data_str);
void zenithra_read_command(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_check_commands(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_run_through(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_free_variable_list(struct InEngineData *engine_data_str, struct InterpreterVariable **head);
struct InterpreterVariable* zenithra_interpreter_create_variable_node(struct InEngineData *engine_data_str);
struct InterpreterVariable* zenithra_interpreter_match_variable_name(struct InEngineData *engine_data_str, char *variable_name);

//-----------------------------------------------
// Interpreter commands Funcs
//-----------------------------------------------

void zenithra_interpreter_command_register_variable(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_command_call_script(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_update_variable(struct InEngineData *engine_data_str, char *file_name);


//-----------------------------------------------
// Deprecated
//-----------------------------------------------

uint64_t zenithra_8_byte_to_int(char *str);