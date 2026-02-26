#include "zenithra_core.h"
#include "zenithra_debug.h"

bool program_should_quit = false;

void zenithra_test_editor(struct InEngineData *engine_data_str);

int main(int argc, char *argv[]) {
    DEV_MODE = true;

    struct InEngineData *engine_data_str = zenithra_init(1200, 700);

    zenithra_test_editor(engine_data_str);

    zenithra_destroy(engine_data_str);
    return 0;
}

void zenithra_test_editor(struct InEngineData *engine_data_str) {
    zenithra_load_object(engine_data_str, "cube.zbj");

    do {
        SDL_SetRenderDrawColor(engine_data_str->SDL->renderer, 0, 0, 0, 255);
        SDL_RenderClear(engine_data_str->SDL->renderer);

        zenithra_render_object(engine_data_str, 0);
        zenithra_draw(engine_data_str);

        program_should_quit = zenithra_handle_event_poll(engine_data_str);

        SDL_RenderPresent(engine_data_str->SDL->renderer);

        zenithra_check_and_display_memory_change(engine_data_str);
    } while (!program_should_quit);
}