#include"zenithra_core.h"

void zenithra_read_console_input(struct InEngineData *engine_data_str, struct ObjectData **obj){
    char c, input_buffer[255];
    memset(input_buffer, 0, sizeof(input_buffer));
    int input_buffer_n = 0;
    
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
            obj[engine_data_str->obj_num] = zenithra_load_obj(engine_data_str, false, input_buffer, "./gamedata/textures/gravel.DDS");
        }
        memset(input_buffer, 0, sizeof(input_buffer));
        input_buffer_n = 0;
    }
}

uint64_t zenithra_8_byte_to_int(char *str){
    int i = 0;
    uint64_t value = 0;
    while(str[i]){
        value = (value << 8) | (unsigned char)str[i];
        i++;
    }
    value = value % 1024;
    return value;
}