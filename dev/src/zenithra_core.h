#include <stdint.h>
#ifdef __linux__
#include <SDL2/SDL_syswm.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <execinfo.h>
#include <unistd.h>
#else
#include <conio.h>
#include <fileapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include "zenithra_debug.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <time.h>

extern bool program_should_quit;
extern bool _dev_mode;

// Flags
#define DEV_MODE                                                                                             \
    0x01                      // Pass to zenithra_init() if creating a console (on Windows) is desired
                              // along-side dev statistics like memory usage
#define BACKFACE_CULLING 0x02 // Pass to zenithra_init() if we want backface culling

#ifdef _WIN32
#if DEV_MODE
#define DEV_CONSOLE_CREATE                                                                                   \
    ({                                                                                                       \
        AllocConsole();                                                                                      \
        freopen("CONOUT$", "wb", stdout);                                                                    \
        freopen("CONIN$", "rb", stdin);                                                                      \
        freopen("CONOUT$", "wb", stderr);                                                                    \
    })
#else
#define DEV_CONSOLE_CREATE
#endif
#else
#define DEV_CONSOLE_CREATE
#endif

//-----------------------------------------------
// Graphics Structs and Defs
//-----------------------------------------------

struct TempNormCoords {
    float norm_X;
    float norm_Y;
};

typedef struct ObjectVerticeData {
    double x, y, z;
    size_t size;
} ZBJ_VERTICE_DATA;

typedef struct ObjectFaceData {
    uint32_t f1, f2, f3;
    size_t size;
} ZBJ_FACE_DATA;

typedef struct ObjectList {
    ZBJ_FACE_DATA *face;
    ZBJ_VERTICE_DATA *vertice;
} ZBJ_LIST;

//-----------------------------------------------
// Movement Structs
//-----------------------------------------------

typedef struct MovementEngineData {
    double cam_X;
    double cam_Y;
    double cam_Z;

    float cam_yaw_rad;   // Rotates around Y-axis
    float cam_pitch_rad; // Rotates around X-axis

    float vFOV_rad;
    float hFOV_rad;

    double Z_near;
    double Z_far;

    float look_speed;

    float cam_speed;

    float relaxed_walk_speed;
    float walk_speed;
    float run_speed;
    float inertia;

    uint32_t carry_weight;
    uint32_t weight;
} MOVE;

//-----------------------------------------------
// Core Structs
//-----------------------------------------------

typedef struct SDLEngineData {
    SDL_Window *window;
    SDL_Renderer *renderer;
} SDL;

struct InEngineData {
    MOVE *MOVE;
    SDL *SDL;
    ZBJ_LIST *ZBJ_LIST;

    SDL_Texture *frame_texture;

    unsigned int obj_number;

    int window_X;
    int window_Y;

    int renderer_X;
    int renderer_Y;

    bool focus_lost;

    uint64_t mem_usage;
    uint64_t old_mem_usage;
};

//-----------------------------------------------
// Core Funcs
//-----------------------------------------------

#ifndef WIN32 // If not on Windows define a function for reading from console input. If on windows such
              // fuction is provided from a header
int _kbhit();
#endif

void zenithra_signal_handle(int sig);
void zenithra_free(struct InEngineData *engine_data_str, void **pp, size_t size);
struct InEngineData *zenithra_init(int X, int Y, int flags);
void zenithra_destroy(struct InEngineData *engine_data_str);
void zenithra_critical_error_occured(struct InEngineData *engine_data_str,
    char *file_name,
    int line,
    const char *error);
void zenithra_initialize_sdl(struct InEngineData *engine_data_str);
void zenithra_disable_bypass_compositor(SDL_Window *window);
void *zenithra_malloc(struct InEngineData *engine_data_str, size_t size, char *_file, int _line);
void zenithra_check_and_display_memory_change(struct InEngineData *engine_data_str);
void *zenithra_realloc(struct InEngineData *engine_data_str,
    void *p,
    size_t new_size,
    size_t old_size,
    char *_file,
    int _line);

//-----------------------------------------------
// Movement Funcs
//-----------------------------------------------

void zenithra_init_movement_vals(struct InEngineData *engine_data_str);
void zenithra_calculate_yaw_pitch(struct InEngineData *engine_data_str);

//-----------------------------------------------
// Events Funcs
//-----------------------------------------------

bool zenithra_handle_event_poll(struct InEngineData *engine_data_str);

//-----------------------------------------------
// Graphics Funcs
//-----------------------------------------------

void zenithra_save_points_for_rendering(struct InEngineData *engine_data_str,
    double Xw,
    double Yw,
    double Zw);
int zenithra_load_object(struct InEngineData *engine_data_str, char *file_name);
void zenithra_destroy_object(struct InEngineData *engine_data_str, int index);
struct TempNormCoords *
zenithra_normalize_vertice(struct InEngineData *engine_data_str, double Xw, double Yw, double Zw);
void zenithra_render_object(struct InEngineData *engine_data_str, int index);