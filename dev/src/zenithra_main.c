#include "zenithra_core.h"
#include "zenithra_debug.h"

void zenithra_test_editor(struct InEngineData *engine_data_str);

int main(int argc, char *argv[]) {
    struct InEngineData *engine_data_str = zenithra_init(1200, 700, DEV_MODE | BACKFACE_CULLING);

    zenithra_test_editor(engine_data_str);

    zenithra_destroy(engine_data_str);
    return 0;
}

void zenithra_test_editor(struct InEngineData *engine_data_str) {
    zenithra_load_object(engine_data_str, "cube.zbj");

    struct timespec old_time, cur_time;
    double elapsed_ns, fps;
    int frame_count = 0;

    clock_gettime(CLOCK_MONOTONIC, &old_time); // initialize

    do {
        clock_gettime(CLOCK_MONOTONIC, &cur_time);

        // elapsed time in nanoseconds
        elapsed_ns = (cur_time.tv_sec - old_time.tv_sec) * 1e9 + (cur_time.tv_nsec - old_time.tv_nsec);

        if (elapsed_ns > 0) {
            fps = 1e9 / elapsed_ns; // frames per second
        } else {
            fps = 0;
        }

        if (frame_count == 100) {
            printf("%.2f\n", fps);
            frame_count = 0;
        }

        old_time = cur_time;
        frame_count++;

        program_should_quit = zenithra_handle_event_poll(engine_data_str);
        zenithra_calculate_yaw_pitch(engine_data_str);

        SDL_RenderClear(engine_data_str->SDL->renderer);
        zenithra_render_object(engine_data_str, 0);
        // zenithra_draw_points(engine_data_str);

        // SDL_RenderCopy(engine_data_str->SDL->renderer, engine_data_str->frame_texture, NULL, NULL);
        SDL_RenderPresent(engine_data_str->SDL->renderer);

        zenithra_check_and_display_memory_change(engine_data_str);
    } while (!program_should_quit);
}