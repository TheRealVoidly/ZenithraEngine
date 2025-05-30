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

    int i_value;
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

    int obj_num;

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

struct ObjectData{
    long long obj_size;
    GLuint obj_normal_buffer;
    GLuint obj_vertex_buffer;
    GLuint obj_uv_buffer;
    GLuint obj_bound_texture;

    vec3 translation_vector;

    GLfloat *vertex_buffer_data;
    GLfloat *bounded_vertex_buffer_data;

    GLfloat x_max, x_average, y_max, y_average, z_max, z_average;

    bool engine_obj;
};

GLuint zenithra_load_shaders(struct InEngineData *engine_data_str);
GLuint zenithra_create_texture(const char* file_name);
struct ObjectData* zenithra_load_obj(struct InEngineData *engine_data_str, bool engine_obj, const char* object_file_name, const char* texture_file_name);
void zenithra_render_object(struct InEngineData *engine_data_str, struct ObjectData **obj, int obj_num);
void zenithra_rebind_texture(struct ObjectData **obj, int obj_num, const char* file_name);
void zenithra_bind_objects(struct ObjectData **obj, int objects_to_be_bound[255], int target_object);
void zenithra_unbind_objects(struct ObjectData *obj);
bool zenithra_alloc_new_obj(struct InEngineData *engine_data_str, struct ObjectData **obj);

//-----------------------------------------------
// Editor
//-----------------------------------------------

int* zenithra_object_ray_intersects_detection(float origin[3], struct ObjectData **obj, struct InEngineData *engine_data_str);
struct ObjectData** zenithra_editor_init(struct InEngineData *engine_data_str);
void zenithra_move_object(struct InEngineData *engine_data_str, struct ObjectData **obj, int obj_num, char vector, GLfloat value);

//-----------------------------------------------
// Interpreter
//-----------------------------------------------

void zenithra_interpreter_begin(struct InEngineData *engine_data_str, struct ObjectData **obj);
void zenithra_register_callback(struct InEngineData *engine_data_str, char *callback_name, char *callback_request);
void zenithra_interpreter_loop(struct InEngineData *engine_data_str, struct ObjectData **obj);
void zenithra_read_command(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_check_commands(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name);
void zenithra_interpreter_run_through(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name);
void zenithra_interpreter_free_variable_list(struct InterpreterVariable **head);
struct InterpreterVariable* zenithra_interpreter_create_variable_node();
struct InterpreterVariable* zenithra_interpreter_match_variable_name(struct InEngineData *engine_data_str, char *variable_name);

//-----------------------------------------------
// Interpreter commands
//-----------------------------------------------

void zenithra_interpreter_command_register_variable(struct InEngineData *engine_data_str, char *file_name);
void zenithra_interpreter_command_load_object(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name);
void zenithra_interpreter_command_bind_texture_to_object(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name);
void zenithra_interpreter_command_move_object(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name);
void zenithra_interpreter_command_call_script(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name);

//-----------------------------------------------
// Deprecated
//-----------------------------------------------

uint64_t zenithra_8_byte_to_int(char *str);