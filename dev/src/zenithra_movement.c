#include "zenithra_core.h"

void zenithra_init_movement_vals(struct InEngineData *engine_data_str) {
    engine_data_str->MOVE->cam_X = 2.0;
    engine_data_str->MOVE->cam_Y = -1.5;
    engine_data_str->MOVE->cam_Z = -4.0;

    engine_data_str->MOVE->cam_yaw_rad = 0.0;   // Rotates around Y-axis
    engine_data_str->MOVE->cam_pitch_rad = 0.0; // Rotates around X-axis

    engine_data_str->MOVE->vFOV_rad = 90 * M_PI / 180.0;
    double aspect = (double)engine_data_str->renderer_X / (double)engine_data_str->renderer_Y;
    engine_data_str->MOVE->hFOV_rad = 2.0 * atan(aspect * tan(engine_data_str->MOVE->vFOV_rad / 2.0));

    engine_data_str->MOVE->Z_far = engine_data_str->MOVE->cam_Z + 100.0;
    engine_data_str->MOVE->Z_near = engine_data_str->MOVE->cam_Z + 0.01;
}