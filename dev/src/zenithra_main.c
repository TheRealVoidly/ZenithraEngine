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
    char c, input_buffer[255];
    memset(input_buffer, 0, sizeof(input_buffer));
    int input_buffer_n = 0;

    struct ObjectData **obj, **temp;
    obj = (struct ObjectData**)malloc(4 * sizeof(struct ObjectData));
    obj[0] = zenithra_load_obj(engine_data_str, true, "./enginedata/vectorarrows/vectorarrowx.obj");
    obj[1] = zenithra_load_obj(engine_data_str, true, "./enginedata/vectorarrows/vectorarrowy.obj");
    obj[2] = zenithra_load_obj(engine_data_str, true, "./enginedata/vectorarrows/vectorarrowz.obj");
    //obj[3] = zenithra_load_obj(engine_data_str, false, "./gamedata/objects/tifa.obj");
    //obj[4] = zenithra_load_obj(engine_data_str, false, "./gamedata/objects/plane.obj");
    GLuint tex_red = zenithra_create_texture("./enginedata/colors/red.DDS");
    GLuint tex_green = zenithra_create_texture("./enginedata/colors/green.DDS");
    GLuint tex_blue = zenithra_create_texture("./enginedata/colors/blue.DDS");
    //GLuint tex_giga = zenithra_create_texture("./gamedata/textures/mugshot1.DDS");
    //GLuint tex_gravel = zenithra_create_texture("./gamedata/textures/gravel.DDS");

    int prev_object_num = engine_data_str->obj_num;
    do{
        last_frame_time = current_frame_time;
        current_frame_time = SDL_GetPerformanceCounter();
        engine_data_str->delta_time = (double)((current_frame_time - last_frame_time) * 1000 / (double)SDL_GetPerformanceFrequency());

        c = '\0';
        if(_kbhit()){
            c = getchar();
            input_buffer[input_buffer_n] = c;
            input_buffer_n++;
        }

        if(c == '\n'){
            if(strncmp(input_buffer, "load_object", 11) == 0){
                for(int i = 0; i < input_buffer_n; i++){
                    if(input_buffer[i+11] != '\n'){
                        input_buffer[i] = input_buffer[i+11];
                    }
                }
                input_buffer[input_buffer_n-12] = '\0';
                obj[engine_data_str->obj_num] = zenithra_load_obj(engine_data_str, false, input_buffer);
            }
            memset(input_buffer, 0, sizeof(input_buffer));
            input_buffer_n = 0;
        }

        if(prev_object_num != engine_data_str->obj_num){
            temp = (struct ObjectData**)realloc(obj, sizeof(struct ObjectData) * (engine_data_str->obj_num+1));
            if(temp != NULL){
                obj = temp;
                prev_object_num = engine_data_str->obj_num;
            }else{
                zenithra_log_err(__FILE__, __LINE__, "Object realloc failed! Object will not be loaded!\n");
            }
        }

        Uint32 mouse_button_pressed = SDL_GetMouseState(NULL, NULL);
        if(!engine_data_str->SDL->focus_lost){
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

        int *object_ray = zenithra_object_ray_intersects_detection(engine_data_str->MOVE->position, obj, engine_data_str);
        if(object_ray[0] == 1 && mouse_button_pressed != SDL_BUTTON(3)){
            zenithra_render_object(engine_data_str, obj, 0, tex_red);
            zenithra_render_object(engine_data_str, obj, 1, tex_green);
            zenithra_render_object(engine_data_str, obj, 2, tex_blue);
            if(mouse_button_pressed == SDL_BUTTON(1)){
                for(int i = 1; i <= (obj[object_ray[1]]->obj_size*3+48)-3; i=i+3){
                    obj[object_ray[1]]->vertex_buffer_data[i] += 0.1f;
                }
                glBindBuffer(GL_ARRAY_BUFFER, obj[object_ray[1]]->obj_vertex_buffer);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj[object_ray[1]]->obj_size * 3, &obj[object_ray[1]]->vertex_buffer_data[0], GL_STATIC_DRAW);
            }
        }
        zenithra_free((void**)&object_ray);

        for(int i = 0; i < engine_data_str->obj_num; i++){
            if(!obj[i]->engine_obj){
                zenithra_render_object(engine_data_str, obj, i, 0);
            }
        }
        //zenithra_render_object(engine_data_str, obj, 3, 0);
        //zenithra_render_object(engine_data_str, obj, 4, tex_gravel);

        SDL_GL_SwapWindow(engine_data_str->SDL->window);
    }while(!program_should_quit);

    for(int i = 0; i < engine_data_str->obj_num; i++){
        glDeleteBuffers(1, &obj[i]->obj_vertex_buffer);
        glDeleteBuffers(1, &obj[i]->obj_uv_buffer);
        zenithra_free((void**)&obj[i]);
    }
}