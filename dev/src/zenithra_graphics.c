#include "zenithra_core.h"

/**
 * Creates the renderer buffer
 * Stores pixel coordinate and color of
 * each pixel
 **/

void zenithra_create_point_buffer(struct InEngineData *engine_data_str) {
    int count = engine_data_str->renderer_X * engine_data_str->renderer_Y;

    POINT_BUF *head = NULL;
    POINT_BUF *next_in_line = NULL;

    for (int i = 0; i < count; i++) {
        POINT_BUF *node = zenithra_malloc(engine_data_str, sizeof *node);

        if (!node) {
            char error_message[255];
            snprintf(error_message,
                sizeof error_message,
                "POINT_BUF allocation "
                "failed at pixel %d",
                i);
            zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, error_message);
        }

        node->X = i % engine_data_str->renderer_X; // Pixel
                                                   // #X left
                                                   // ->
                                                   // right
        node->Y = i / engine_data_str->renderer_X; // Pixel
                                                   // #Y up
                                                   // -> down

        node->r = 0; // Initialize all
                     // pixels as black
        node->g = 0;
        node->b = 0;
        node->w = 0;

        node->next = NULL;

        if (!head) {
            head = node;
        } else {
            next_in_line->next = node;
        }

        next_in_line = node;
    }
    engine_data_str->POINT_BUF = head;

    zenithra_log_msg("Point buffer created "
                     "successfully");
}

/**
 * Destroy the point buffer
 * Probably only when terminating the
 * engine
 **/

void zenithra_destroy_point_buffer(struct InEngineData *engine_data_str) {
    POINT_BUF *node = engine_data_str->POINT_BUF;
    POINT_BUF *next_in_line = node->next;

    zenithra_free(engine_data_str, (void **)&node, sizeof *node);

    while (next_in_line) {
        node = next_in_line;
        next_in_line = node->next;

        zenithra_free(engine_data_str, (void **)&node, sizeof *node);
    }
}

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
zenithra_normalize_vertice(struct InEngineData *engine_data_str, double Xw, double Yw, double Zw) {
    double Xr = Xw - engine_data_str->MOVE->cam_X; // Point
                                                   // coordinates
                                                   // relative to
                                                   // camera
    double Yr = Yw - engine_data_str->MOVE->cam_Y;
    double Zr = Zw - engine_data_str->MOVE->cam_Z;

    double X_cam = Xr * cos(engine_data_str->MOVE->cam_yaw_rad) -
        Zr * sin(engine_data_str->MOVE->cam_yaw_rad); // Yaw rotation around Y-axis
    double Y_cam = Yr;
    double Z_cam =
        Xr * sin(engine_data_str->MOVE->cam_yaw_rad) + Zr * cos(engine_data_str->MOVE->cam_yaw_rad);

    double Y_cam2 = Y_cam * cos(engine_data_str->MOVE->cam_pitch_rad) -
        Z_cam * sin(engine_data_str->MOVE->cam_pitch_rad); // Pitch rotation around X-axis
    double Z_cam2 =
        Y_cam * sin(engine_data_str->MOVE->cam_pitch_rad) + Z_cam * cos(engine_data_str->MOVE->cam_pitch_rad);
    Y_cam = Y_cam2;
    Z_cam = Z_cam2;

    if (Z_cam < engine_data_str->MOVE->Z_near) {
        // Point is between camerra and
        // the near plane return NULL;
    }
    if (Z_cam > engine_data_str->MOVE->Z_far) {
        // Point is behind the far plane
        // return NULL;
    }

    double max_X_at_Z = Z_cam * tan(engine_data_str->MOVE->hFOV_rad / 2.0); // Find maximum
                                                                            // visible X at Z_cam
    double max_Y_at_Z = Z_cam * tan(engine_data_str->MOVE->vFOV_rad / 2.0); // Find maximum
                                                                            // visible Y at Z_cam
    if (X_cam < -max_X_at_Z || X_cam > max_X_at_Z || Y_cam < -max_Y_at_Z || Y_cam > max_Y_at_Z) {
        // Point is outside FOV
        // return NULL;
    }

    struct TempNormCoords *temp_norm_coords = NULL;
    temp_norm_coords = zenithra_malloc(engine_data_str, sizeof *temp_norm_coords);

    temp_norm_coords->norm_X = (X_cam / max_X_at_Z + 1.0) / 2.0; // Normalize X and Y to
                                                                 // [0,1]
    temp_norm_coords->norm_Y = (Y_cam / max_Y_at_Z + 1.0) / 2.0;

    return temp_norm_coords;
}

/**
 * Translate to normalized screen
 * coordinated via
 * zenithra_calculate_and_normalize_vertice()
 * and then to pixels coordinates and
 * save into
 * engine_data_str->POINT_RENDERER_BUF
 *
 * @param double Xw, double Yw, double
 * Zw world coordinates of vertice
 **/

void zenithra_save_points_for_rendering(struct InEngineData *engine_data_str,
    double Xw,
    double Yw,
    double Zw) {
    struct TempNormCoords *temp_norm_coords;
    temp_norm_coords = zenithra_normalize_vertice(engine_data_str, Xw, Yw, Zw);

    int screen_X = (int)round(temp_norm_coords->norm_X * engine_data_str->renderer_X); // Map to
                                                                                       // screen
                                                                                       // pixels
    int screen_Y = (int)round(temp_norm_coords->norm_Y * engine_data_str->renderer_Y);

    zenithra_free(engine_data_str, (void **)&temp_norm_coords, sizeof *temp_norm_coords);

    POINT_BUF *node = engine_data_str->POINT_BUF;
    while (node) {
        if (screen_X == node->X && screen_Y == node->Y) {
            node->r = 255;
        }

        node = node->next;
    }
}

/**
 * Render an object
 *
 * @param index of object
 **/

void zenithra_render_object(struct InEngineData *engine_data_str, int index) {}

/**
 * Draws all the pixels stored in the
 * engine_data_str->RENDERER_BUF
 **/

void zenithra_draw(struct InEngineData *engine_data_str) {
    POINT_BUF *node = engine_data_str->POINT_BUF;
    while (node) {
        SDL_SetRenderDrawColor(engine_data_str->SDL->renderer, node->r, node->g, node->b, node->w);
        SDL_RenderDrawPoint(engine_data_str->SDL->renderer, node->X, node->Y);

        node = node->next;
    }
}

/**
 * Function initializes memory for new
 * object in ZBJ_LIST and saves all the
 * data for that object Index of new
 * object is always
 * engine_data_str->obj_number
 *
 * @return object index if object loaded
 * properly IF error occurs, return -1
 * and free all successfully allocated
 * memory for object
 **/

int zenithra_load_object(struct InEngineData *engine_data_str, char *file_name) {
    FILE *fp;

    fp = fopen(file_name, "rb");

    if (!fp) {
        char error_message[255];
        snprintf(error_message,
            sizeof error_message,
            "Object file: %s does not "
            "exist",
            file_name);
        zenithra_log_err(__FILE__, __LINE__, error_message);
        return false;
    }

    char line[255];
    float x, y, z;
    int f1, f2, f3;

    ZBJ_LIST *list_node = NULL;

    if (engine_data_str->obj_number == 0) {
        list_node = zenithra_malloc(engine_data_str, sizeof *list_node);
    } else {
        list_node = zenithra_realloc(engine_data_str,
            engine_data_str->ZBJ_LIST,
            (engine_data_str->obj_number + 1) * sizeof *list_node,
            (engine_data_str->obj_number) * sizeof *list_node);
    }

    if (!list_node) {
        char error_message[255];
        snprintf(error_message,
            sizeof error_message,
            "Object allocation failed "
            "for object n: %d",
            engine_data_str->obj_number);
        zenithra_log_err(__FILE__, __LINE__, error_message);
        zenithra_destroy_object(engine_data_str, engine_data_str->obj_number);
        return false;
    }

    engine_data_str->ZBJ_LIST = list_node;

    ZBJ_VERTICE_DATA *tmp_vertice_data = NULL;
    ZBJ_FACE_DATA *tmp_face_data = NULL;

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
                if (n_vertices == 1) {
                    tmp_vertice_data =
                        zenithra_malloc(engine_data_str, (n_vertices) * sizeof *tmp_vertice_data);

                    if (tmp_vertice_data) {
                        engine_data_str->ZBJ_LIST[engine_data_str->obj_number].vertice = tmp_vertice_data;
                    } else {
                        char error_message[255];
                        snprintf(error_message,
                            sizeof error_message,
                            "Vertice "
                            "n: %d "
                            "allocation"
                            " for "
                            "object n: "
                            "%d failed",
                            n_vertices,
                            engine_data_str->obj_number);
                        zenithra_log_err(__FILE__, __LINE__, error_message);
                        zenithra_destroy_object(engine_data_str, engine_data_str->obj_number);
                        return false;
                    }
                } else {
                    tmp_vertice_data = zenithra_realloc(engine_data_str,
                        engine_data_str->ZBJ_LIST[engine_data_str->obj_number].vertice,
                        (n_vertices) * sizeof *tmp_vertice_data,
                        (n_vertices - 1) * sizeof *tmp_vertice_data);
                    tmp_vertice_data->size = (n_vertices) * sizeof *tmp_vertice_data;

                    if (tmp_vertice_data) {
                        engine_data_str->ZBJ_LIST[engine_data_str->obj_number].vertice = tmp_vertice_data;
                    } else {
                        char error_message[255];
                        snprintf(error_message,
                            sizeof error_message,
                            "Vertice "
                            "n: %d "
                            "allocation"
                            " for "
                            "object n: "
                            "%d failed",
                            n_vertices,
                            engine_data_str->obj_number);
                        zenithra_log_err(__FILE__, __LINE__, error_message);
                        zenithra_destroy_object(engine_data_str, engine_data_str->obj_number);
                        return false;
                    }
                }

                tmp_vertice_data = NULL;

                if (sscanf(line, "%f %f %f", &x, &y, &z) == 3) {
                    engine_data_str->ZBJ_LIST[engine_data_str->obj_number].vertice[n_vertices - 1].x = x;
                    engine_data_str->ZBJ_LIST[engine_data_str->obj_number].vertice[n_vertices - 1].y = y;
                    engine_data_str->ZBJ_LIST[engine_data_str->obj_number].vertice[n_vertices - 1].z = z;

                    n_vertices++;
                }
            }

            if (stage == 1) {
                if (n_faces == 1) {
                    tmp_face_data = zenithra_malloc(engine_data_str, (n_faces) * sizeof *tmp_face_data);

                    if (tmp_face_data) {
                        engine_data_str->ZBJ_LIST[engine_data_str->obj_number].face = tmp_face_data;
                    } else {
                        char error_message[255];
                        snprintf(error_message,
                            sizeof error_message,
                            "Vertice "
                            "n: %d "
                            "allocation"
                            " for "
                            "object n: "
                            "%d failed",
                            n_faces,
                            engine_data_str->obj_number);
                        zenithra_log_err(__FILE__, __LINE__, error_message);
                        zenithra_destroy_object(engine_data_str, engine_data_str->obj_number);
                        return false;
                    }
                } else {
                    tmp_face_data = zenithra_realloc(engine_data_str,
                        engine_data_str->ZBJ_LIST[engine_data_str->obj_number].face,
                        (n_faces) * sizeof *tmp_face_data,
                        (n_faces - 1) * sizeof *tmp_face_data);
                    tmp_face_data->size = (n_faces) * sizeof *tmp_face_data;

                    if (tmp_face_data) {
                        engine_data_str->ZBJ_LIST[engine_data_str->obj_number].face = tmp_face_data;
                    } else {
                        char error_message[255];
                        snprintf(error_message,
                            sizeof error_message,
                            "Vertice "
                            "n: %d "
                            "allocation"
                            " for "
                            "object n: "
                            "%d failed",
                            n_faces,
                            engine_data_str->obj_number);
                        zenithra_log_err(__FILE__, __LINE__, error_message);
                        zenithra_destroy_object(engine_data_str, engine_data_str->obj_number);
                        return false;
                    }
                }

                tmp_face_data = NULL;

                if (sscanf(line, "%d %d %d", &f1, &f2, &f3) == 3) {
                    engine_data_str->ZBJ_LIST[engine_data_str->obj_number].face[n_faces - 1].f1 = f1;
                    engine_data_str->ZBJ_LIST[engine_data_str->obj_number].face[n_faces - 1].f2 = f2;
                    engine_data_str->ZBJ_LIST[engine_data_str->obj_number].face[n_faces - 1].f3 = f3;

                    n_faces++;
                }
            }
        }
    }

    char message[255];
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
                zenithra_free(engine_data_str,
                    (void **)&engine_data_str->ZBJ_LIST[i].face,
                    engine_data_str->ZBJ_LIST[i].face->size);
                zenithra_free(engine_data_str,
                    (void **)&engine_data_str->ZBJ_LIST[i].vertice,
                    engine_data_str->ZBJ_LIST[i].vertice->size);
            }

            zenithra_free(engine_data_str,
                (void **)&engine_data_str->ZBJ_LIST,
                (sizeof *engine_data_str->ZBJ_LIST) * (engine_data_str->obj_number));
        }
    } else {
        if (engine_data_str->obj_number < index) {
            zenithra_free(engine_data_str,
                (void **)&engine_data_str->ZBJ_LIST[index].face,
                engine_data_str->ZBJ_LIST[index].face->size);
            zenithra_free(engine_data_str,
                (void **)&engine_data_str->ZBJ_LIST[index].vertice,
                engine_data_str->ZBJ_LIST[index].vertice->size);

            for (int i = index; i < engine_data_str->obj_number; i++) {
                if (i + 1 < engine_data_str->obj_number) {
                    engine_data_str->ZBJ_LIST[i] = engine_data_str->ZBJ_LIST[i + 1];
                }
            }

            if (engine_data_str->obj_number > 1) {
                engine_data_str->ZBJ_LIST = zenithra_realloc(engine_data_str,
                    engine_data_str->ZBJ_LIST,
                    (engine_data_str->obj_number - 1) * sizeof *engine_data_str->ZBJ_LIST,
                    (engine_data_str->obj_number) * sizeof *engine_data_str->ZBJ_LIST);
            } else {
                zenithra_free(engine_data_str,
                    (void **)&engine_data_str->ZBJ_LIST,
                    (sizeof *engine_data_str->ZBJ_LIST) * (engine_data_str->obj_number));
            }
            engine_data_str->obj_number--;
        }
    }
}