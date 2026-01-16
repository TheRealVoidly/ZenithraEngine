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

#define GLEW_STATIC

#include<GL/glew.h>
#include<SDL2/SDL_opengl.h>
#include<cglm/cglm.h>
#include"zenithra_debug.h"

#define DEV_MODE // Define if creating a console (on Windows) is desired

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
// Core
//-----------------------------------------------

typedef struct SDLEngineData{
    SDL_Window *window;
    SDL_Renderer *renderer;
}SDL;

typedef struct GLEngineData{
    GLuint program_id;
    GLuint matrix_id;
    GLuint vertex_array_id;
}GL;

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

typedef struct KeysEngineData{
    bool escape;
    bool r_shift;
}KEYS;

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

struct InEngineData{
    MOVE *MOVE;
    SDL *SDL;   
    GL *GL;
    KEYS *KEYS;
    INTERPRETER *INTERPRETER;

    float delta_time;

    int window_x;
    int window_y;

    bool focus_lost;
};

#ifndef WIN32 // If not on Windows define a function for reading from console input. If on windows such fuction is provided from a header
int _kbhit();
#endif

void zenithra_signal_catch(int n);
void zenithra_free(void **pp);
struct InEngineData* zenithra_init(int x, int y);
void zenithra_destroy(struct InEngineData *engine_data_str);
void zenithra_critical_error_occured(struct InEngineData *engine_data_str, char* file_name, int line, const char* error);
bool zenithra_initialize_opengl(struct InEngineData *engine_data_str);
bool zenithra_initialize_sdl(struct InEngineData *engine_data_str);
void zenithra_init_keys(struct InEngineData *engine_data_str);
void zenithra_disable_bypass_compositor(SDL_Window *window);

//-----------------------------------------------
// Movement
//-----------------------------------------------

void zenithra_init_movement_vals(struct InEngineData *engine_data_str);
void zenithra_calc_mouse_movement(struct InEngineData *engine_data_str);
void zenithra_update_position(struct InEngineData *engine_data_str);

//-----------------------------------------------
// Events
//-----------------------------------------------

bool zenithra_handle_event_poll(struct InEngineData *engine_data_str);

//-----------------------------------------------
// Graphics
//-----------------------------------------------



//-----------------------------------------------
// Editor
//-----------------------------------------------

#define START_OF_OBJECT_INDEX 1

int* zenithra_object_ray_intersects_detection(float origin[3], struct InEngineData *engine_data_str);

//-----------------------------------------------
// Interpreter
//-----------------------------------------------

void zenithra_interpreter_begin(struct InEngineData *engine_data_str);
void zenithra_register_callback(struct InEngineData *engine_data_str, char *callback_name, char *callback_request);
void zenithra_interpreter_loop(struct InEngineData *engine_data_str);
void zenithra_read_command(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_check_commands(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_run_through(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_free_variable_list(struct InterpreterVariable **head);
struct InterpreterVariable* zenithra_interpreter_create_variable_node();
struct InterpreterVariable* zenithra_interpreter_match_variable_name(struct InEngineData *engine_data_str, char *variable_name);

//-----------------------------------------------
// Interpreter commands
//-----------------------------------------------

void zenithra_interpreter_command_register_variable(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_command_call_script(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_update_variable(struct InEngineData *engine_data_str, char *file_name);

//-----------------------------------------------
// Deprecated
//-----------------------------------------------

uint64_t zenithra_8_byte_to_int(char *str);