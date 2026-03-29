#include "zenithra_core.h"
#include <signal.h>

bool program_should_quit = false;
bool _dev_mode = false;
bool _x11 = false;

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
 * @note If initialization fails at any point, zenithra_critical_error_occured is called, and the program will
 * terminate.
 **/

struct InEngineData *zenithra_init(int x, int y, int flags, char *font_path, int font_size) {
    signal(SIGSEGV, zenithra_signal_handle);
    signal(SIGINT, zenithra_signal_handle);
    signal(SIGTERM, zenithra_signal_handle);

    zenithra_log_init();

    struct InEngineData *engine_data_str = malloc(sizeof *engine_data_str);
    if (!engine_data_str) {
        zenithra_critical_error_occured(NULL, __FILE__, __LINE__, "engine_data_str memmory alloc failed");
    }

    engine_data_str->mem_usage = 0;
    engine_data_str->old_mem_usage = 0;

    engine_data_str->SDL = zenithra_malloc(engine_data_str, sizeof *engine_data_str->SDL, __FILE__, __LINE__);

    engine_data_str->MOVE =
        zenithra_malloc(engine_data_str, sizeof *engine_data_str->MOVE, __FILE__, __LINE__);

    engine_data_str->TIMER =
        zenithra_malloc(engine_data_str, sizeof *engine_data_str->TIMER, __FILE__, __LINE__);

    engine_data_str->focus_lost = false;  // Window starts in focus
    engine_data_str->fps_enabled = false; // FPS counter invisible

    zenithra_log_msg("Zenithra engine started");

    engine_data_str->window_x = x;
    engine_data_str->window_y = y;

    if (flags & DEV_MODE) {
        _dev_mode = true;
        DEV_CONSOLE_CREATE // Creates developer console if DEV_MODE is set. Only on Windows
    }

    zenithra_initialize_sdl(engine_data_str);
    zenithra_log_msg("SDL initialized successfully");

    engine_data_str->font = TTF_OpenFont(font_path, font_size);
    if (!engine_data_str->font) {
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, TTF_GetError());
    }
    zenithra_log_msg("Font initialized successfully");

    zenithra_init_movement_vals(engine_data_str);
    engine_data_str->TIMER->update_old_time = time(NULL);

    engine_data_str->obj_number = 0;

    zenithra_log_msg("Zenithra initialized successfully");

    return engine_data_str;
}

void zenithra_initialize_sdl(struct InEngineData *engine_data_str) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

    engine_data_str->SDL->window = SDL_CreateWindow("Zenithra Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        engine_data_str->window_x,
        engine_data_str->window_y,
        SDL_WINDOW_SHOWN);
    if (!engine_data_str->SDL->window) {
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }

    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");

    engine_data_str->SDL->renderer =
        SDL_CreateRenderer(engine_data_str->SDL->window, -1, SDL_RENDERER_ACCELERATED);
    if (!engine_data_str->SDL->renderer) {
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, SDL_GetError());
    }
    SDL_GetRendererOutputSize(
        engine_data_str->SDL->renderer, &engine_data_str->renderer_x, &engine_data_str->renderer_y);

    if (_x11) {
        zenithra_disable_bypass_compositor(engine_data_str->SDL->window);
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetRenderDrawBlendMode(engine_data_str->SDL->renderer, SDL_BLENDMODE_BLEND);

    engine_data_str->frame_texture = SDL_CreateTexture(engine_data_str->SDL->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        engine_data_str->renderer_x,
        engine_data_str->renderer_y);

    if (TTF_Init() == -1) {
        zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, TTF_GetError());
    }
}

/**
 * Ends the engine
 * Shall be called only at the end of program as this terminates the process
 *
 * Frees and destroys everything allocated
 *
 * @param engine_data_str = Needed to be passed to the function to free allocated in-engine struct
 **/

void zenithra_destroy(struct InEngineData *engine_data_str) {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_SetWindowGrab(engine_data_str->SDL->window, SDL_FALSE);

    SDL_DestroyTexture(engine_data_str->frame_texture);
    SDL_DestroyRenderer(engine_data_str->SDL->renderer);
    SDL_DestroyWindow(engine_data_str->SDL->window);

    TTF_CloseFont(engine_data_str->font);

    TTF_Quit();
    SDL_Quit();

    zenithra_destroy_object(engine_data_str, -1);

    zenithra_free(engine_data_str, (void **)&engine_data_str->TIMER, sizeof *engine_data_str->TIMER);
    zenithra_free(engine_data_str, (void **)&engine_data_str->MOVE, sizeof *engine_data_str->MOVE);
    zenithra_free(engine_data_str, (void **)&engine_data_str->SDL, sizeof *engine_data_str->SDL);

    zenithra_check_and_display_memory_change(engine_data_str);

    free(engine_data_str);

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

void zenithra_critical_error_occured(struct InEngineData *engine_data_str,
    char *file_name,
    int line,
    const char *error) {
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

void zenithra_free(struct InEngineData *engine_data_str, void **pp, size_t size) {
    if (!*pp) {
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

void zenithra_signal_handle(int sig) {
    char *t_buffer;
    int fd;

#ifdef __linux__
    void *buffer[15];
    char **callstack;
    int frames;
    int i;
#endif

    switch (sig) {
    case SIGSEGV:
#ifdef __linux__
        frames = backtrace(buffer, 15);
        callstack = backtrace_symbols(buffer, frames);

        fd = open("./dev/zenithra_log.txt", O_APPEND | O_WRONLY | O_CREAT, 0644);

        for (i = frames - 1; i > 0; i--) {
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

#ifdef __linux__

/**
 * Function defined only for Linux
 * Disables compositor bypass if one is active
 **/

void zenithra_disable_bypass_compositor(SDL_Window *window) {
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(window, &wmInfo)) {
        SDL_Log("SDL_GetWindowWMInfo failed: %s", SDL_GetError());
        zenithra_log_err(__FILE__, __LINE__, "Failed to disable compositor bypass");
        return;
    }

    Display *display = wmInfo.info.x11.display;
    Window xwindow = wmInfo.info.x11.window;

    Atom bypass = XInternAtom(display, "_NET_WM_BYPASS_COMPOSITOR", False);
    if (bypass) {
        unsigned long value = 0;
        XChangeProperty(
            display, xwindow, bypass, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&value, 1);
        XFlush(display);
    }
}

#endif

/**
 * Should be used for dynamic allocation instead of malloc for memory logging purposes
 **/

void *zenithra_malloc(struct InEngineData *engine_data_str, size_t size, char *_file, int _line) {
    void *ret_p;
    ret_p = malloc(size);
    if (!ret_p) {
        zenithra_critical_error_occured(engine_data_str, _file, _line, "Malloc failed!");
        return NULL;
    }
    engine_data_str->mem_usage += size;
    return ret_p;
}

/**
 * Should be used for dynamic reallocation instead of realloc for memory logging purposes
 **/

void *zenithra_realloc(struct InEngineData *engine_data_str,
    void *p,
    size_t new_size,
    size_t old_size,
    char *_file,
    int _line) {
    void *ret_p;
    ret_p = realloc(p, new_size);
    if (!ret_p) {
        zenithra_critical_error_occured(engine_data_str, _file, _line, "Realloc failed!");
        return NULL;
    }
    engine_data_str->mem_usage -= old_size;
    engine_data_str->mem_usage += new_size;
    return ret_p;
}

/**
 * Checks if the memory usage changed since last check and displays the new value in MB
 **/

void zenithra_check_and_display_memory_change(struct InEngineData *engine_data_str) {
    if (engine_data_str->mem_usage != engine_data_str->old_mem_usage) {
        if (_dev_mode) {
            printf("%f\n", (float)((float)engine_data_str->mem_usage / 1048576.0));
        }
        engine_data_str->old_mem_usage = engine_data_str->mem_usage;
    }
}

/**
 * Fps counter function
 **/

SDL_Texture *zenithra_update_and_display_fps(struct InEngineData *engine_data_str) {
    double elapsed_ns, fps;

    SDL_Surface *surf;

    SDL_Texture *tex;

    SDL_Color font_color;
    font_color.r = 255;
    font_color.g = 255;
    font_color.b = 255;
    font_color.a = 255;

    clock_gettime(CLOCK_MONOTONIC, &engine_data_str->TIMER->fps_cur_time);

    // elapsed time in nanoseconds
    elapsed_ns =
        (engine_data_str->TIMER->fps_cur_time.tv_sec - engine_data_str->TIMER->fps_old_time.tv_sec) * 1e9 +
        (engine_data_str->TIMER->fps_cur_time.tv_nsec - engine_data_str->TIMER->fps_old_time.tv_nsec);

    if (elapsed_ns > 0) {
        fps = 1e9 / elapsed_ns; // frames per second
    } else {
        fps = 0;
    }

    engine_data_str->TIMER->fps_old_time = engine_data_str->TIMER->fps_cur_time;

    if (time(NULL) - engine_data_str->TIMER->update_old_time >= 1) {
        engine_data_str->TIMER->update_old_time = time(NULL);
        char fps_string[256];

        sprintf(fps_string, "%.f", fps);
        surf = TTF_RenderUTF8_Solid(engine_data_str->font, fps_string, font_color);

        tex = SDL_CreateTextureFromSurface(engine_data_str->SDL->renderer, surf);
        SDL_FreeSurface(surf);
        return tex;
    }

    return NULL;
}

/**
 * Function should be called once every iteration of the main loop
 * Takes care of all engine functionalities
 **/

void zenithra_update(struct InEngineData *engine_data_str) {
    SDL_Rect fps_rect; // FPS counter position
    fps_rect.x = 0;
    fps_rect.y = 0;
    fps_rect.w = 80;
    fps_rect.h = 20;

    SDL_Texture *temp_fps_texture;
    SDL_Texture *long_fps_texture;

    program_should_quit = zenithra_handle_event_poll(engine_data_str); // Events and inputs
    zenithra_calculate_yaw_pitch(engine_data_str);

    SDL_RenderClear(engine_data_str->SDL->renderer);

    zenithra_render_object(engine_data_str, 0); // This may be game resposibility, Idk yet

    if (engine_data_str->fps_enabled) { // FPS counter self explanatory
        if ((temp_fps_texture = zenithra_update_and_display_fps(engine_data_str))) {
            SDL_DestroyTexture(long_fps_texture);
            long_fps_texture = temp_fps_texture;
        }
        SDL_RenderCopy(engine_data_str->SDL->renderer, long_fps_texture, NULL, &fps_rect);
    }

    SDL_RenderPresent(engine_data_str->SDL->renderer); // Render frame

    zenithra_check_and_display_memory_change(engine_data_str); // Memory usage tracking

    if (program_should_quit) {
        SDL_DestroyTexture(long_fps_texture);
    }
}