#include"zenithra_core.h"
#include"zenithra_debug.h"

/**
 * Function used for object detection
 * 
 * @param origin = position of camera
 * @param obj = pointer to array of objects
 * @param engine_data_str
 * @return int array of size 2, index 0 stores whether an object has been hit by a ray, index 1 stores the index of an object that has been hit
**/

int* zenithra_object_ray_intersects_detection(float origin[3], struct ObjectData **obj, struct InEngineData *engine_data_str){
    int *n;
    float dir[3], edge1[3], edge2[3], h[3], s[3], q[3];
    n = malloc(sizeof(int) * 2);
    n[0] = 0;
    n[1] = 0;

    float length = sqrtf((engine_data_str->MOVE->direction_look[0]*engine_data_str->MOVE->direction_look[0]) + (engine_data_str->MOVE->direction_look[1]*engine_data_str->MOVE->direction_look[1]) + (engine_data_str->MOVE->direction_look[2]*engine_data_str->MOVE->direction_look[2]));
    dir[0] = (engine_data_str->MOVE->direction_look[0] /= length);
    dir[1] = (engine_data_str->MOVE->direction_look[1] /= length);
    dir[2] = (engine_data_str->MOVE->direction_look[2] /= length);

    float temp = 1e30f;

    for(int j = START_OF_OBJECT_INDEX; j < engine_data_str->obj_num; j++){
        //if(!obj[j]->engine_obj){
            for(int i = 0; i < obj[j]->obj_size * 9; i+=3){
                float *v0 = &obj[j]->vertex_buffer_data[i+0];
                float *v1 = &obj[j]->vertex_buffer_data[i+3];
                float *v2 = &obj[j]->vertex_buffer_data[i+6];

                for(int k = 0; k < 3; k++){
                    edge1[k] = v1[k] - v0[k];
                }
                for(int k = 0; k < 3; k++){
                    edge2[k] = v2[k] - v0[k];
                }
                h[0] = dir[1]*edge2[2] - dir[2]*edge2[1];
                h[1] = dir[2]*edge2[0] - dir[0]*edge2[2];
                h[2] = dir[0]*edge2[1] - dir[1]*edge2[0];

                float a = edge1[0]*h[0] + edge1[1]*h[1] + edge1[2]*h[2];
                if(fabs(a) < 0.0000001){
                    continue;
                }

                float f = 1.0 / a;
                for(int k = 0; k < 3; k++){
                    s[k] = origin[k] - v0[k];
                }

                float u = f * (s[0]*h[0] + s[1]*h[1] + s[2]*h[2]);
                if(u < 0.0 || u > 1.0){
                    continue;
                }

                q[0] = s[1]*edge1[2] - s[2]*edge1[1];
                q[1] = s[2]*edge1[0] - s[0]*edge1[2];
                q[2] = s[0]*edge1[1] - s[1]*edge1[0];

                float v = f * (dir[0]*q[0] + dir[1]*q[1] + dir[2]*q[2]);
                if(v < 0.0 || u + v > 1.0){
                    continue;
                }

                float t = f * (edge2[0]*q[0] + edge2[1]*q[1] + edge2[2]*q[2]);
                if(t > 1e-5){
                    if(t < temp){
                        temp = t;
                        n[0] = 1;
                        n[1] = j;
                    }
                }
            }
        //}
    }
    return n;
}

/**
 * Initializes the editor
 * 
 * @param engine_data_str
 * @return object structure pointer
**/

struct ObjectData** zenithra_editor_init(struct InEngineData *engine_data_str){
    engine_data_str->EDITOR->selected_object = 0;

    struct ObjectData **obj;
    obj = (struct ObjectData**)malloc(4 * sizeof(struct ObjectData));

    obj[0] = NULL;
    obj[ENGINE_OBJ_X_ARROW] = zenithra_load_obj(engine_data_str, true, "./enginedata/vectorarrows/vectorarrowx.obj", "./enginedata/colors/red.DDS");
    obj[ENGINE_OBJ_Y_ARROW] = zenithra_load_obj(engine_data_str, true, "./enginedata/vectorarrows/vectorarrowy.obj", "./enginedata/colors/green.DDS");
    obj[ENGINE_OBJ_Z_ARROW] = zenithra_load_obj(engine_data_str, true, "./enginedata/vectorarrows/vectorarrowz.obj", "./enginedata/colors/blue.DDS");

    return obj;
}

/**
 * Function for moving objects around
 * 
 * @param engine_data_str
 * @param obj
 * @param obj_num
 * @param vector x/y/z
 * @param value by how much an object is going to be moved
**/

void zenithra_move_object(struct InEngineData *engine_data_str, struct ObjectData **obj, int obj_num, char vector, GLfloat value){
    int i;

    switch(vector){
    case 'x':
        for(i = 0; i <= (obj[obj_num]->obj_size * 9) - 3; i=i+3){
            obj[obj_num]->vertex_buffer_data[i] += value;
        }
        obj[obj_num]->x_average += value;
        obj[obj_num]->x_max += value;

        break;
    case 'y':
        for(i = 0; i <= (obj[obj_num]->obj_size * 9) - 3; i=i+3){
            obj[obj_num]->vertex_buffer_data[i+1] += value;
        }
        obj[obj_num]->y_average += value;
        obj[obj_num]->y_max += value;

        break;
    case 'z':
        for(i = 0; i <= (obj[obj_num]->obj_size * 9) - 3; i=i+3){
            obj[obj_num]->vertex_buffer_data[i+2] += value;
        }
        obj[obj_num]->z_average += value;
        obj[obj_num]->z_max += value;

        break;
    default:
        zenithra_log_msg("Not a valid vector was specified when trying to move an object");
        break;
    }

    glBindBuffer(GL_ARRAY_BUFFER, obj[obj_num]->obj_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(GLfloat) * obj[obj_num]->obj_size * 9), &obj[obj_num]->vertex_buffer_data[0], GL_STATIC_DRAW);
}

/**
 * Function for scaling objects in size
 * 
 * @param engine_data_str
 * @param obj
 * @param obj_num
 * @param value by how much an object is going to be scaled in all directions from the epicentre
**/

void zenithra_scale_object(struct InEngineData *engine_data_str, struct ObjectData **obj, int obj_num, GLfloat value){

}