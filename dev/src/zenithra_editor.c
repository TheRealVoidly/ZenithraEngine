#include"zenithra_core.h"
#include"zenithra_debug.h"

int* zenithra_object_ray_intersects_detection(float origin[3], struct ObjectData **obj, struct InEngineData *engine_data_str){
    int *n;
    float dir[3], edge1[3], edge2[3], h[3], s[3], q[3];
    n = malloc(sizeof(int) * 2);
    n[0] = 0;
    n[1] = 0;

    if(!obj[engine_data_str->obj_num-1]){
        return n;
    }

    float length = sqrtf((engine_data_str->MOVE->direction_look[0]*engine_data_str->MOVE->direction_look[0]) + (engine_data_str->MOVE->direction_look[1]*engine_data_str->MOVE->direction_look[1]) + (engine_data_str->MOVE->direction_look[2]*engine_data_str->MOVE->direction_look[2]));
    dir[0] = (engine_data_str->MOVE->direction_look[0] /= length);
    dir[1] = (engine_data_str->MOVE->direction_look[1] /= length);
    dir[2] = (engine_data_str->MOVE->direction_look[2] /= length);

    float temp = 1e30f;

    for(int j = 0; j < engine_data_str->obj_num; j++){
        if(!obj[j]->engine_obj){
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
        }
    }
    return n;
}