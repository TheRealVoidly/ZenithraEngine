#include"zenithra_core.h"

void zenithra_init_movement_vals(struct InEngineData *engine_data_str){
	engine_data_str->MOVE->cam_x = 0.0f;
	engine_data_str->MOVE->cam_y = 2.0f;
	engine_data_str->MOVE->cam_z = 2.0f;

	engine_data_str->MOVE->cam_facing_horizontal_angle = 0.0f;
	engine_data_str->MOVE->cam_facing_vertical_angle = 0.0f;

	engine_data_str->MOVE->vFOV = 90;
	engine_data_str->MOVE->hFOV = ((float)16 / (float)9) * (float)engine_data_str->MOVE->vFOV;

	engine_data_str->MOVE->z_far = engine_data_str->MOVE->cam_z + 5.0f;
	engine_data_str->MOVE->z_near = engine_data_str->MOVE->cam_z + 0.01f;
}