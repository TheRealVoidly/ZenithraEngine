#include "zenithra_core.h"
#include <signal.h>

bool program_should_quit = false;
bool _dev_mode = false;
bool _show_fps = false;

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

struct InEngineData *zenithra_init(int X, int Y, int flags, char *font_path, int font_size) {
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

    engine_data_str->focus_lost = false; // Window starts in focus

    zenithra_log_msg("Zenithra engine started");

    engine_data_str->window_X = X;
    engine_data_str->window_Y = Y;

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
        engine_data_str->window_X,
        engine_data_str->window_Y,
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
        engine_data_str->SDL->renderer, &engine_data_str->renderer_X, &engine_data_str->renderer_Y);

#ifdef __linux__
    zenithra_disable_bypass_compositor(engine_data_str->SDL->window);
#endif

    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetRenderDrawBlendMode(engine_data_str->SDL->renderer, SDL_BLENDMODE_BLEND);

    engine_data_str->frame_texture = SDL_CreateTexture(engine_data_str->SDL->renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        engine_data_str->renderer_X,
        engine_data_str->renderer_Y);

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
 * Handles real-time console input
 **/

int _kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds); // File descriptor 0 = stdin
    return select(1, &fds, NULL, NULL, &tv);
}

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

    if (_show_fps && engine_data_str->) {
        char fps_string[256];

        sprintf(fps_string, "%.f", fps);
        return SDL_CreateTextureFromSurface(engine_data_str->SDL->renderer,
            TTF_RenderText_Solid(engine_data_str->font, fps_string, font_color));
    }

    return NULL;
}

/**
 * Initializes fps counter
 **/

void zenithra_initialize_timer(struct InEngineData *engine_data_str) {
    engine_data_str->TIMER->update_cur_time = time(NULL);
}