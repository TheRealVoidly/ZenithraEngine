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

    do{
        last_frame_time = current_frame_time;
        current_frame_time = SDL_GetPerformanceCounter();
        engine_data_str->delta_time = (double)((current_frame_time - last_frame_time) * 1000 / (double)SDL_GetPerformanceFrequency());

        /*Uint32 mouse_button_pressed = SDL_GetMouseState(NULL, NULL);
        if(!engine_data_str->focus_lost){
            zenithra_calc_mouse_movement(engine_data_str);

            if(SDL_BUTTON(3) == mouse_button_pressed){
                zenithra_update_position(engine_data_str);
            }
        }*/

        RENDERER_BUF *next_in_line = engine_data_str->RENDERER_BUF;
        while(next_in_line){
            SDL_SetRenderDrawColor(engine_data_str->SDL->renderer, next_in_line->r, next_in_line->g, next_in_line->b, next_in_line->w);
            SDL_RenderDrawPoint(engine_data_str->SDL->renderer, next_in_line->x, next_in_line->y);

            next_in_line = next_in_line->next;
        }


        program_should_quit = zenithra_handle_event_poll(engine_data_str);

        SDL_RenderPresent(engine_data_str->SDL->renderer);
    }while(!program_should_quit);
}