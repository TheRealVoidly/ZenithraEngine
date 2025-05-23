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

void zenithra_bind_objects(struct ObjectData **obj, int objects_to_be_bound[255], int target_object){
    if(obj[target_object]->bounded_vertex_buffer_data){
        zenithra_log_err(__FILE__, __LINE__, "Objects could not be bounded to target object.. Object already bounded to\n");
        return;
    }

    long long objects_size = 0;
    for(int i = 0; i < 255; i+=2){
        if(objects_to_be_bound[i] == -1){
            break;
        }
        objects_size += obj[objects_to_be_bound[i]]->obj_size * 3;
    }
    obj[target_object]->bounded_vertex_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * ((obj[target_object]->obj_size * 3) + (objects_size)));

    GLfloat x_max = obj[target_object]->vertex_buffer_data[0];
    GLfloat x_average = obj[target_object]->vertex_buffer_data[0];
    GLfloat y_max = obj[target_object]->vertex_buffer_data[1];
    GLfloat y_average = obj[target_object]->vertex_buffer_data[1];
    GLfloat z_max = obj[target_object]->vertex_buffer_data[2];
    GLfloat z_average = obj[target_object]->vertex_buffer_data[2];

    int n = 0;
    for(int i = 3; i < obj[target_object]->obj_size * 3; i+=3){
        if(obj[target_object]->vertex_buffer_data[i] > x_max){
            x_max = obj[target_object]->vertex_buffer_data[i];
        }
        if(obj[target_object]->vertex_buffer_data[i+1] > y_max){
            y_max = obj[target_object]->vertex_buffer_data[i];
        }
        if(obj[target_object]->vertex_buffer_data[i+2] > z_max){
            z_max = obj[target_object]->vertex_buffer_data[i];
        }

        n++;
        x_average += obj[target_object]->vertex_buffer_data[i];
        y_average += obj[target_object]->vertex_buffer_data[i+1];
        z_average += obj[target_object]->vertex_buffer_data[i+2];

        obj[target_object]->bounded_vertex_buffer_data[i] = obj[target_object]->vertex_buffer_data[i];
        obj[target_object]->bounded_vertex_buffer_data[i+1] = obj[target_object]->vertex_buffer_data[i+1];
        obj[target_object]->bounded_vertex_buffer_data[i+2] = obj[target_object]->vertex_buffer_data[i+2];
    }

    x_average /= n;
    y_average /= n;
    z_average /= n;

    for(int i = 0; i < 255; i+=2){
        if(objects_to_be_bound[i] == -1){
            break;
        }

        n = 0;
        for(int j = obj[target_object]->obj_size * 3; j < (obj[target_object]->obj_size * 3) + objects_size; j+=3){
            switch(objects_to_be_bound[i+1]){
            case 1:
                obj[target_object]->bounded_vertex_buffer_data[j] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + x_max;
                obj[target_object]->bounded_vertex_buffer_data[j+1] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + y_average;
                obj[target_object]->bounded_vertex_buffer_data[j+2] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + z_average;
                break;
            case 2:
                obj[target_object]->bounded_vertex_buffer_data[j] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + x_average;
                obj[target_object]->bounded_vertex_buffer_data[j+1] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + y_max;
                obj[target_object]->bounded_vertex_buffer_data[j+2] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + z_average;
                break;
            case 3:
                obj[target_object]->bounded_vertex_buffer_data[j] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + x_average;
                obj[target_object]->bounded_vertex_buffer_data[j+1] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + y_average;
                obj[target_object]->bounded_vertex_buffer_data[j+2] = obj[objects_to_be_bound[i]]->vertex_buffer_data[n] + z_max;
                break;
            }
            n++;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, obj[target_object]->obj_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) * obj[target_object]->obj_size * 3) + (objects_size), &obj[target_object]->bounded_vertex_buffer_data[0], GL_STATIC_DRAW);
}

void zenithra_unbind_objects(struct ObjectData *obj){
    zenithra_free((void**)&obj->bounded_vertex_buffer_data);
    glBindBuffer(GL_ARRAY_BUFFER, obj->obj_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) * obj->obj_size * 3), &obj->vertex_buffer_data[0], GL_STATIC_DRAW);
}