#include "zenithra_core.h"
#include "zenithra_debug.h"

void zenithra_test_editor(struct InEngineData *engine_data_str);

int main(int argc, char *argv[]) {
    struct InEngineData *engine_data_str =
        zenithra_init(1200, 700, DEV_MODE | BACKFACE_CULLING, "JetBrainsMonoNerdFont-Regular.ttf", 28);

    zenithra_test_editor(engine_data_str);

    zenithra_destroy(engine_data_str);
    return 0;
}

void zenithra_test_editor(struct InEngineData *engine_data_str) {
    SDL_Texture *temp_fps_texture;
    SDL_Texture *long_fps_texture;

    zenithra_load_object(engine_data_str, "cube.zbj");

    _show_fps = true;
    do {
        program_should_quit = zenithra_handle_event_poll(engine_data_str);
        zenithra_calculate_yaw_pitch(engine_data_str);

        SDL_RenderClear(engine_data_str->SDL->renderer);
        zenithra_render_object(engine_data_str, 0);

        if ((temp_fps_texture = zenithra_update_and_display_fps(engine_data_str))) {
            long_fps_texture = temp_fps_texture;
        }
        SDL_RenderCopy(engine_data_str->SDL->renderer, long_fps_texture, NULL, NULL);
        // SDL_RenderCopy(engine_data_str->SDL->renderer, engine_data_str->frame_texture, NULL, NULL);
        SDL_RenderPresent(engine_data_str->SDL->renderer);

        zenithra_check_and_display_memory_change(engine_data_str);
    } while (!program_should_quit);
}