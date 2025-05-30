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

    struct ObjectData **obj = zenithra_editor_init(engine_data_str);
    zenithra_interpreter_begin(engine_data_str, obj);

    do{
        last_frame_time = current_frame_time;
        current_frame_time = SDL_GetPerformanceCounter();
        engine_data_str->delta_time = (double)((current_frame_time - last_frame_time) * 1000 / (double)SDL_GetPerformanceFrequency());

        Uint32 mouse_button_pressed = SDL_GetMouseState(NULL, NULL);
        if(!engine_data_str->focus_lost){
            zenithra_calc_mouse_movement(engine_data_str);

            if(SDL_BUTTON(3) == mouse_button_pressed){
                zenithra_update_position(engine_data_str);
            }
        }
        program_should_quit = zenithra_handle_event_poll(engine_data_str);

        glClearColor(0.03f, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(SDL_BUTTON(3) != mouse_button_pressed){
            glUseProgram(0); 
            glPointSize(3.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glEnd();
        }
        glUseProgram(engine_data_str->GL->program_id);

        zenithra_interpreter_loop(engine_data_str, obj);

        int *object_ray = zenithra_object_ray_intersects_detection(engine_data_str->MOVE->position, obj, engine_data_str);
        if(object_ray[0] == 1 && mouse_button_pressed != SDL_BUTTON(3)){
            zenithra_unbind_objects(obj[0]);
            zenithra_unbind_objects(obj[1]);
            zenithra_unbind_objects(obj[2]);

            int temp_array[255] = {0, 0, 1, 1, 2, 2, -1};
            zenithra_bind_objects(obj, temp_array, object_ray[1]);

            zenithra_render_object(engine_data_str, obj, 0);
            zenithra_render_object(engine_data_str, obj, 1);
            zenithra_render_object(engine_data_str, obj, 2);
            if(mouse_button_pressed == SDL_BUTTON(1)){
                //zenithra_move_object(engine_data_str, obj, object_ray[1], 'x', 0.01f);
            }
        }
        zenithra_free((void**)&object_ray);

        for(int i = 0; i < engine_data_str->obj_num; i++){
            if(!obj[i]->engine_obj){
                zenithra_render_object(engine_data_str, obj, i);
            }
        }

        SDL_GL_SwapWindow(engine_data_str->SDL->window);
    }while(!program_should_quit);

    for(int i = 0; i < engine_data_str->obj_num; i++){
        glDeleteBuffers(1, &obj[i]->obj_vertex_buffer);
        glDeleteBuffers(1, &obj[i]->obj_uv_buffer);
        zenithra_free((void**)&obj[i]);
    }
}