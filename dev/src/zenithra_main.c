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


        int test_x[10000];
        int test_y[10000];

        int l = 0;



        for(int x = -10; x <= 10; x++){
            for(float z = 0.1; z <= 2; z = z + 0.1){
                test_x[l] = (((((-1.0f * ((float)x - engine_data_str->MOVE->cam_x)) * 0.01f) / (-1.0f * (z - engine_data_str->MOVE->cam_z))) + 1.0f) / 2.0f) * engine_data_str->renderer_x;
                test_y[l] = (((((-1.0f * (0.1f - engine_data_str->MOVE->cam_y)) * 0.01f) / (-1.0f * (z - engine_data_str->MOVE->cam_z))) + 1.0f) / 2.0f) * engine_data_str->renderer_y;

                l++;
            }
        }

        int max = l;
        RENDERER_BUF *node = engine_data_str->RENDERER_BUF;
        while(node){
            for(l = 0; l < max; l++){
                if(test_x[l] == node->x && test_y[l] == node->y){
                    node->r = 255;
                }
            }

            node = node->next;
        }


        node = engine_data_str->RENDERER_BUF;
        while(node){
            SDL_SetRenderDrawColor(engine_data_str->SDL->renderer, node->r, node->g, node->b, node->w);
            SDL_RenderDrawPoint(engine_data_str->SDL->renderer, node->x, node->y);

            node = node->next;
        }


        program_should_quit = zenithra_handle_event_poll(engine_data_str);

        SDL_RenderPresent(engine_data_str->SDL->renderer);
    }while(!program_should_quit);
}