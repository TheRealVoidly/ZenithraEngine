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

typedef struct SDLEngineData{
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool focus_lost;
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

struct InEngineData{
    MOVE *MOVE;
    SDL *SDL;
    GL *GL;
    KEYS *KEYS;

    float delta_time;
    int window_x;
    int window_y;

    int obj_num;
};

void zenithra_signal_catch(int n);
void zenithra_free(void **pp);
void zenithra_free_list(void **head);
void* zenithra_create_node(void **node, bool is_head, int struct_type_size);
struct InEngineData* zenithra_init(int x, int y);
void zenithra_destroy(struct InEngineData *engine_data_str);
void zenithra_critical_error_occured(struct InEngineData *engine_data_str, char* file_name, int line, const char* error);
bool zenithra_initialize_opengl(struct InEngineData *engine_data_str);
bool zenithra_initialize_sdl(struct InEngineData *engine_data_str);
void zenithra_init_keys(struct InEngineData *engine_data_str);

//-----------------------------------------------
//Movement
//-----------------------------------------------

void zenithra_init_movement_vals(struct InEngineData *engine_data_str);
void zenithra_calc_mouse_movement(struct InEngineData *engine_data_str);
void zenithra_update_position(struct InEngineData *engine_data_str);

//-----------------------------------------------
//Events
//-----------------------------------------------

bool zenithra_handle_event_poll(struct InEngineData *engine_data_str);

//-----------------------------------------------
//Graphics
//-----------------------------------------------

struct ObjectData{
    long long obj_size;
    GLuint obj_normal_buffer;
    GLuint obj_vertex_buffer;
    GLuint obj_uv_buffer;

    vec3 translation_vector;

    GLfloat *vertex_buffer_data;

    bool engine_obj;
};

GLuint zenithra_load_shaders(struct InEngineData *engine_data_str);
GLuint zenithra_create_texture(const char* file_name);
struct ObjectData* zenithra_load_obj(struct InEngineData *engine_data_str, bool engine_obj, const char* file_name);
void zenithra_render_object(struct InEngineData *engine_data_str, struct ObjectData **obj, int obj_num, GLuint tex_id);

//-----------------------------------------------
//Editor
//-----------------------------------------------

int* zenithra_object_ray_intersects_detection(float origin[3], struct ObjectData **obj, struct InEngineData *engine_data_str);