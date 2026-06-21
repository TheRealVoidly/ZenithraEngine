#include "zenithra_core.h"

static void _free_object(struct InEngineData *engine_data_str, int index);
static void _read_object(struct InEngineData *engine_data_str, int stage, int n, char line[256]);
static void _set_vertice(struct InEngineData *engine_data_str, SDL_Vertex *vertices, int index, int i);

/**
 * Check if point is renderable/should
 * be rendered and translate it to
 * normalized screen coordinates
 *
 * @param Xw, Yw, Zw world coordinates
 * of the point
 * @return struct TempNormCoords*
 * pointer to normalized coordinates
 * save in temporary struct, NULL when
 * point should not be rendered
 **/

struct TempNormCoords *
zenithra_normalize_vertice(struct InEngineData *engine_data_str, double xw, double yw, double zw) {
    double xr = xw - engine_data_str->MOVE->cam_x; // Point coordinates relative to camera
    double yr = yw - engine_data_str->MOVE->cam_y;
    double zr = zw - engine_data_str->MOVE->cam_z;

    double X_cam = xr * cos(engine_data_str->MOVE->cam_yaw_rad) -
        zr * sin(engine_data_str->MOVE->cam_yaw_rad); // Yaw rotation around Y-axis
    double Y_cam = yr;
    double Z_cam =
        xr * sin(engine_data_str->MOVE->cam_yaw_rad) + zr * cos(engine_data_str->MOVE->cam_yaw_rad);

    double Y_cam2 = Y_cam * cos(engine_data_str->MOVE->real_cam_pitch_rad) -
        Z_cam * sin(engine_data_str->MOVE->real_cam_pitch_rad); // Pitch rotation around X-axis
    double Z_cam2 = Y_cam * sin(engine_data_str->MOVE->real_cam_pitch_rad) +
        Z_cam * cos(engine_data_str->MOVE->real_cam_pitch_rad);
    Y_cam = Y_cam2;
    Z_cam = Z_cam2;

    double max_X_at_Z = Z_cam * tan(engine_data_str->MOVE->hFOV_rad / 2.0); // Find maximum visible X at Z_cam
    double max_Y_at_Z = Z_cam * tan(engine_data_str->MOVE->vFOV_rad / 2.0); // Find maximum visible Y at Z_cam

    struct TempNormCoords *temp_norm_coords = NULL;
    temp_norm_coords = zenithra_malloc(engine_data_str, sizeof *temp_norm_coords, __FILE__, __LINE__);

    temp_norm_coords->out_of_view = true;

    if (-X_cam < -max_X_at_Z && -X_cam > max_X_at_Z || -Y_cam < -max_Y_at_Z && -Y_cam > max_Y_at_Z) {
        if (X_cam < -max_X_at_Z || X_cam > max_X_at_Z || Y_cam < -max_Y_at_Z || Y_cam > max_Y_at_Z) {
            // Point is outside FOV && NOT behind camera

            temp_norm_coords->out_of_view = true;
            temp_norm_coords->norm_x = (X_cam / max_X_at_Z + 1.0) / 2.0; // Normalize X and Y to [0,1]
            temp_norm_coords->norm_y = (Y_cam / max_Y_at_Z + 1.0) / 2.0;
        } else {
            // Point is inside FOV
            temp_norm_coords->out_of_view = false;
            temp_norm_coords->norm_x = (X_cam / max_X_at_Z + 1.0) / 2.0; // Normalize X and Y to [0,1]
            temp_norm_coords->norm_y = (Y_cam / max_Y_at_Z + 1.0) / 2.0;
        }
    }

    return temp_norm_coords;
}

/**
 * Render an object
 *
 * @param index of object
 **/

void zenithra_render_object(struct InEngineData *engine_data_str, int index) {
    int num_of_faces =
        (engine_data_str->OBJ_LIST[index].face->size / sizeof *engine_data_str->OBJ_LIST[index].face);

    SDL_Vertex *vertices =
        zenithra_malloc(engine_data_str, num_of_faces * 3 * sizeof *vertices, __FILE__, __LINE__);

    for (int i = 0; i < num_of_faces * 3; i++) {
        _set_vertice(engine_data_str, vertices, index, i);
    }

    // int *indices = zenithra_malloc(engine_data_str, num_of_faces * sizeof *indices, __FILE__, __LINE__);

    SDL_RenderGeometry(engine_data_str->SDL->renderer, NULL, vertices, num_of_faces * 3, NULL, 0);

    zenithra_free(engine_data_str, (void **)&vertices, num_of_faces * 3 * sizeof *vertices);
    // zenithra_free(engine_data_str, (void **)&indices, num_of_faces * sizeof *indices);
}

/**
 * Function initializes memory for new
 * object in OBJ_LIST and saves all the
 * data for that object Index of new
 * object is always
 * engine_data_str->obj_number
 *
 * @return object index if object loaded
 * properly, return -1 if file does not exist
 **/

int zenithra_load_object(struct InEngineData *engine_data_str, char *file_name) {
    FILE *fp;

    fp = fopen(file_name, "rb");

    if (!fp) {
        char error_message[256];
        snprintf(error_message,
            sizeof error_message,
            "Object file: %s does not "
            "exist",
            file_name);
        zenithra_log_err(__FILE__, __LINE__, error_message);
        return -1;
    }

    OBJ_LIST *list_node = NULL;

    if (engine_data_str->obj_number == 0) {
        list_node = zenithra_malloc(engine_data_str, sizeof *list_node, __FILE__, __LINE__);
    } else {
        list_node = zenithra_realloc(engine_data_str,
            engine_data_str->OBJ_LIST,
            (engine_data_str->obj_number + 1) * sizeof *list_node,
            (engine_data_str->obj_number) * sizeof *list_node,
            __FILE__,
            __LINE__);
    }

    engine_data_str->OBJ_LIST = list_node;

    char line[256];
    int n_vertices = 1;
    int n_faces = 1;
    int stage = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == 'f' || line[0] == 'v') {
            if (line[0] == 'f') {
                stage = 1;
            }
        } else {
            if (stage == 0) {
                _read_object(engine_data_str, stage, n_vertices, line);
                n_vertices++;
            }

            if (stage == 1) {
                _read_object(engine_data_str, stage, n_faces, line);
                n_faces++;
            }
        }
    }

    char message[256];
    snprintf(message,
        sizeof message,
        "Object n: %d loaded "
        "successfully",
        engine_data_str->obj_number);
    zenithra_log_msg(message);

    engine_data_str->obj_number++;
    fclose(fp);
    return engine_data_str->obj_number - 1;
}

/**
 * Free all memory allocated to
 * engine_data_str->ZBJ_LIST with index
 *
 * If index = -1, free all objects
 **/

void zenithra_destroy_object(struct InEngineData *engine_data_str, int index) {
    if (index == -1) {
        if (engine_data_str->obj_number > 0) {
            for (int i = 0; i < engine_data_str->obj_number; i++) {
                _free_object(engine_data_str, i);
            }

            zenithra_free(engine_data_str,
                (void **)&engine_data_str->OBJ_LIST,
                (sizeof *engine_data_str->OBJ_LIST) * (engine_data_str->obj_number));
        }
    } else {
        if (engine_data_str->obj_number < index) {
            _free_object(engine_data_str, index);

            for (int i = index; i < engine_data_str->obj_number; i++) {
                if (i + 1 < engine_data_str->obj_number) {
                    engine_data_str->OBJ_LIST[i] = engine_data_str->OBJ_LIST[i + 1];
                }
            }

            if (engine_data_str->obj_number > 1) {
                engine_data_str->OBJ_LIST = zenithra_realloc(engine_data_str,
                    engine_data_str->OBJ_LIST,
                    (engine_data_str->obj_number - 1) * sizeof *engine_data_str->OBJ_LIST,
                    (engine_data_str->obj_number) * sizeof *engine_data_str->OBJ_LIST,
                    __FILE__,
                    __LINE__);
            } else {
                zenithra_free(engine_data_str,
                    (void **)&engine_data_str->OBJ_LIST,
                    (sizeof *engine_data_str->OBJ_LIST) * (engine_data_str->obj_number));
            }
            engine_data_str->obj_number--;
        }
    }
}

//-----------------------------------------------
// Helper funcs
//-----------------------------------------------

static void _free_object(struct InEngineData *engine_data_str, int index) {
    zenithra_free(engine_data_str,
        (void **)&engine_data_str->OBJ_LIST[index].face,
        engine_data_str->OBJ_LIST[index].face->size);
    zenithra_free(engine_data_str,
        (void **)&engine_data_str->OBJ_LIST[index].vertice,
        engine_data_str->OBJ_LIST[index].vertice->size);
}

static void _read_object(struct InEngineData *engine_data_str, int stage, int n, char line[256]) {
    OBJ_VERTICE_DATA *tmp_vertice_data = NULL;
    OBJ_FACE_DATA *tmp_face_data = NULL;
    float x, y, z;
    int f1, f2, f3;
    if (stage == 0) {
        if (n == 1) {
            tmp_vertice_data =
                zenithra_malloc(engine_data_str, (n) * sizeof *tmp_vertice_data, __FILE__, __LINE__);

            engine_data_str->OBJ_LIST[engine_data_str->obj_number].vertice = tmp_vertice_data;
        } else {
            tmp_vertice_data = zenithra_realloc(engine_data_str,
                engine_data_str->OBJ_LIST[engine_data_str->obj_number].vertice,
                (n) * sizeof *tmp_vertice_data,
                (n - 1) * sizeof *tmp_vertice_data,
                __FILE__,
                __LINE__);
            tmp_vertice_data->size = (n) * sizeof *tmp_vertice_data;

            engine_data_str->OBJ_LIST[engine_data_str->obj_number].vertice = tmp_vertice_data;
        }

        tmp_vertice_data = NULL;

        if (sscanf(line, "%f %f %f", &x, &y, &z) == 3) {
            engine_data_str->OBJ_LIST[engine_data_str->obj_number].vertice[n - 1].x = x;
            engine_data_str->OBJ_LIST[engine_data_str->obj_number].vertice[n - 1].y = y;
            engine_data_str->OBJ_LIST[engine_data_str->obj_number].vertice[n - 1].z = z;

            n++;
        }
    }

    if (stage == 1) {
        if (n == 1) {
            tmp_face_data = zenithra_malloc(engine_data_str, (n) * sizeof *tmp_face_data, __FILE__, __LINE__);

            engine_data_str->OBJ_LIST[engine_data_str->obj_number].face = tmp_face_data;
        } else {
            tmp_face_data = zenithra_realloc(engine_data_str,
                engine_data_str->OBJ_LIST[engine_data_str->obj_number].face,
                (n) * sizeof *tmp_face_data,
                (n - 1) * sizeof *tmp_face_data,
                __FILE__,
                __LINE__);
            tmp_face_data->size = (n) * sizeof *tmp_face_data;

            engine_data_str->OBJ_LIST[engine_data_str->obj_number].face = tmp_face_data;
        }

        tmp_face_data = NULL;

        if (sscanf(line, "%d %d %d", &f1, &f2, &f3) == 3) {
            engine_data_str->OBJ_LIST[engine_data_str->obj_number].face[n - 1].f1 = f1;
            engine_data_str->OBJ_LIST[engine_data_str->obj_number].face[n - 1].f2 = f2;
            engine_data_str->OBJ_LIST[engine_data_str->obj_number].face[n - 1].f3 = f3;

            n++;
        }
    }
}

static void _set_vertice(struct InEngineData *engine_data_str, SDL_Vertex *vertices, int index, int i) {
    OBJ_VERTICE_DATA temp;

    if (i % 3 == 0) {
        temp = engine_data_str->OBJ_LIST[index].vertice[engine_data_str->OBJ_LIST[index].face[i / 3].f1 - 1];
    }
    if (i % 3 == 1) {
        temp = engine_data_str->OBJ_LIST[index].vertice[engine_data_str->OBJ_LIST[index].face[i / 3].f2 - 1];
    }
    if (i % 3 == 2) {
        temp = engine_data_str->OBJ_LIST[index].vertice[engine_data_str->OBJ_LIST[index].face[i / 3].f3 - 1];
    }

    struct TempNormCoords *temp_norm_coords;
    temp_norm_coords = zenithra_normalize_vertice(engine_data_str, temp.x, temp.y, temp.z);

    if (!temp_norm_coords->out_of_view) {
        vertices[i].position.x = temp_norm_coords->norm_x * engine_data_str->renderer_x;
        vertices[i].position.y = temp_norm_coords->norm_y * engine_data_str->renderer_y;
        vertices[i].color.r = 255;
        vertices[i].color.g = 0;
        vertices[i].color.b = 0;
        vertices[i].color.a = 255;
    } else {
        vertices[i].position.x = temp_norm_coords->norm_x * engine_data_str->renderer_x;
        vertices[i].position.y = temp_norm_coords->norm_y * engine_data_str->renderer_y;
        vertices[i].color.r = 255;
        vertices[i].color.g = 0;
        vertices[i].color.b = 0;
        vertices[i].color.a = 255;
    }
    zenithra_free(engine_data_str, (void **)&temp_norm_coords, sizeof *temp_norm_coords);
}