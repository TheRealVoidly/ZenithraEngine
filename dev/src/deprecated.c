#include"zenithra_core.h"

void zenithra_free_list(void **head){
    if(!*head){
        zenithra_log_msg("Attempt to free null list");
        return;
    }

    struct TempList{
        struct TempList *next;
        char *temp_arr_fill;
    };

    void *temp;
    while(((void*)(((struct TempList*)*head)->next))){
        temp = *head;
        *head = ((void*)(((struct TempList*)*head)->next));
        zenithra_free(&temp);
        if(!((void*)(((struct TempList*)*head)->next))){
            zenithra_free(head);
            break;
        }
    }
}

void* zenithra_create_node(void **node, bool is_head, int struct_type_size){
    struct TempList{
        struct TempList *next;
        char temp_arr_fill[struct_type_size-8];
    };
    void *new_node;

    if(is_head == true){
        new_node = (void*)malloc(sizeof(struct TempList));
        ((struct TempList*)new_node)->next = NULL;
    }else{
        if(((void*)(((struct TempList*)*node)->next))){
            zenithra_log_msg("Attempt to create node with non-NULL pointer");
            return new_node;
        }
        ((struct TempList*)*node)->next = (void*)malloc(sizeof(struct TempList));
        new_node = ((void*)(((struct TempList*)*node)->next));
        ((struct TempList*)new_node)->next = NULL;
    }
    
    return (struct TempList*)new_node;
}

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