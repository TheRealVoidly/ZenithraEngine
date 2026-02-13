#include"zenithra_core.h"
#include"zenithra_debug.h"

void zenithra_test_editor(struct InEngineData *engine_data_str);

int main(int argc, char *argv[]){
    struct InEngineData *engine_data_str = zenithra_init(1200, 700);

    zenithra_test_editor(engine_data_str);

    zenithra_destroy(engine_data_str);
    return 0;
}

void zenithra_test_editor(struct InEngineData *engine_data_str){
    bool program_should_quit = false;
    Uint64 last_frame_time = 0, current_frame_time = 0;
    float delta_time;

    do{
        last_frame_time = current_frame_time;
        current_frame_time = SDL_GetPerformanceCounter();
        delta_time = (double)((current_frame_time - last_frame_time) * 1000 / (double)SDL_GetPerformanceFrequency());

        for(double i = 0.0; i < 100.0; i++){
            for(double j = 0.0; j < 10.0; j++){
                zenithra_check_and_save_for_rendering(engine_data_str, i, 0.0, j);
            }
        }

        zenithra_draw_points(engine_data_str);

        program_should_quit = zenithra_handle_event_poll(engine_data_str);

        SDL_RenderPresent(engine_data_str->SDL->renderer);
    }while(!program_should_quit);
}