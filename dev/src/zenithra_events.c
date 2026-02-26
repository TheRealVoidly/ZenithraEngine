#include "zenithra_core.h"

bool zenithra_handle_event_poll(struct InEngineData *engine_data_str) {
    SDL_Event event;
    SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            program_should_quit = true;
        }

        if (event.type == SDL_KEYDOWN && !event.key.repeat) {
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && !engine_data_str->focus_lost) {
                SDL_ShowCursor(SDL_ENABLE);
                SDL_SetRelativeMouseMode(SDL_FALSE);
                SDL_SetWindowGrab(engine_data_str->SDL->window, SDL_FALSE);
                engine_data_str->focus_lost = true;

                event.key.keysym.scancode = 0;
            }
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && engine_data_str->focus_lost) {
                SDL_ShowCursor(SDL_DISABLE);
                SDL_SetRelativeMouseMode(SDL_TRUE);
                SDL_SetWindowGrab(engine_data_str->SDL->window, SDL_TRUE);
                engine_data_str->focus_lost = false;

                event.key.keysym.scancode = 0;
            }
        }
    }
    return program_should_quit;
}