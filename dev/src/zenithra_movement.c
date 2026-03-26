#include "zenithra_core.h"

void zenithra_init_movement_vals(struct InEngineData *engine_data_str) {
    engine_data_str->MOVE->cam_x = 2.0;
    engine_data_str->MOVE->cam_y = 1.5;
    engine_data_str->MOVE->cam_z = -4.0;

    engine_data_str->MOVE->cam_yaw_rad = M_PI;   // Rotates around Y-axis
    engine_data_str->MOVE->cam_pitch_rad = M_PI; // Rotates around X-axis

    engine_data_str->MOVE->vFOV_rad = 90 * M_PI / 180.0;
    double aspect = (double)engine_data_str->renderer_x / (double)engine_data_str->renderer_y;
    engine_data_str->MOVE->hFOV_rad = 2.0 * atan(aspect * tan(engine_data_str->MOVE->vFOV_rad / 2.0));

    engine_data_str->MOVE->z_far = engine_data_str->MOVE->cam_z + 100.0;
    engine_data_str->MOVE->z_near = engine_data_str->MOVE->cam_z + 0.01;

    engine_data_str->MOVE->look_speed = 0.001f;

    engine_data_str->MOVE->cam_speed = 2;

    // Character values
    // m / s
    // Values can be conditioned and increased but decreased as well, depending on body weight and
    // fitness level
    engine_data_str->MOVE->relaxed_walk_speed = 1.2; // -0.4% per 1kg
    engine_data_str->MOVE->walk_speed = 1.8;         // -0.6% per 1kg
    engine_data_str->MOVE->run_speed = 3.3;          // -0.8% per 1kg, max running weight usually 32kg
    engine_data_str->MOVE->inertia = 0.0;

    // g
    engine_data_str->MOVE->weight = 80000;       // Default weight 80kg
    engine_data_str->MOVE->carry_weight = 40000; // Default carry weight 40kg
}

void zenithra_calculate_yaw_pitch(struct InEngineData *engine_data_str) {
    int cursor_X[1];
    int cursor_Y[1];

    SDL_PumpEvents();
    if (!engine_data_str->focus_lost) {
        SDL_GetMouseState(&cursor_X[0], &cursor_Y[0]);
        SDL_WarpMouseInWindow(
            engine_data_str->SDL->window, engine_data_str->window_x / 2, engine_data_str->window_y / 2);

        engine_data_str->MOVE->cam_yaw_rad +=
            engine_data_str->MOVE->look_speed * (float)(engine_data_str->window_x / 2.0f - cursor_X[0]);

        engine_data_str->MOVE->cam_pitch_rad -= // Y world is reversed? Idk, too lazy to look into it
            engine_data_str->MOVE->look_speed * (float)(engine_data_str->window_y / 2.0f - cursor_Y[0]);
        if (engine_data_str->MOVE->cam_pitch_rad < 4 * M_PI / 9) {
            engine_data_str->MOVE->cam_pitch_rad = 4 * M_PI / 9;
        }
        if (engine_data_str->MOVE->cam_pitch_rad > 14 * M_PI / 9) {
            engine_data_str->MOVE->cam_pitch_rad = 14 * M_PI / 9;
        }
    }
}

void zenithra_calculate_movement(struct InEngineData *engine_data_str) {
    if (engine_data_str->MOVE->cam_yaw_rad >= 0 && engine_data_str->MOVE->cam_yaw_rad < M_PI / 2.0) {
        // Quadrant I +cos, +sin
    }
    if (engine_data_str->MOVE->cam_yaw_rad >= M_PI / 2.0 && engine_data_str->MOVE->cam_yaw_rad < M_PI) {
        // Quadrant II -cos, +sin
    }
    if (engine_data_str->MOVE->cam_yaw_rad >= M_PI && engine_data_str->MOVE->cam_yaw_rad < 3.0 * M_PI / 2.0) {
        // Quadrant III -cos, -sin
    }
    if (engine_data_str->MOVE->cam_yaw_rad >= 3.0 * M_PI / 2.0 &&
        engine_data_str->MOVE->cam_yaw_rad < 2.0 * M_PI) {
        // Quadrant IV +cos, -sin
    }
    // engine_data_str->MOVE->cam_X +=
}