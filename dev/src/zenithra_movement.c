#include"zenithra_core.h"

void zenithra_init_movement_vals(struct InEngineData *engine_data_str){
	engine_data_str->MOVE->cam_X = 0.0;
	engine_data_str->MOVE->cam_Y = -1.0;
	engine_data_str->MOVE->cam_Z = 0.0;

	engine_data_str->MOVE->cam_yaw = 0.0; //Rotates around Y-axis
	engine_data_str->MOVE->cam_pitch = 0.0; //Rotates around X-axis

	engine_data_str->MOVE->vFOV = 90;
	engine_data_str->MOVE->hFOV = ((float)16 / (float)9) * (float)engine_data_str->MOVE->vFOV;

	engine_data_str->MOVE->Z_far = engine_data_str->MOVE->cam_Z + 100.0;
	engine_data_str->MOVE->Z_near = engine_data_str->MOVE->cam_Z + 0.01;
}