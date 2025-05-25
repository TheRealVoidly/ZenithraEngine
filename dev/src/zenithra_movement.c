#include"zenithra_core.h"

void zenithra_init_movement_vals(struct InEngineData *engine_data_str){
	glm_vec3_make((vec3){4, 0, 4}, engine_data_str->MOVE->position);
	engine_data_str->MOVE->horizontal_angle = 3.14f;
	engine_data_str->MOVE->vertical_angle = 0.0f;
	engine_data_str->MOVE->fov = 90.0f;
	engine_data_str->MOVE->speed = 0.005f;
	engine_data_str->MOVE->gravity = 0.0000001f;
	engine_data_str->MOVE->mouse_speed = 0.001f;
}

void zenithra_calc_mouse_movement(struct InEngineData *engine_data_str){
	//int old_x_pos = engine_data_str->MOVE->x_pos, old_y_pos = engine_data_str->MOVE->y_pos;
	SDL_GetMouseState(&engine_data_str->MOVE->x_pos, &engine_data_str->MOVE->y_pos);
	SDL_WarpMouseInWindow(engine_data_str->SDL->window, engine_data_str->window_x / 2, engine_data_str->window_y / 2);

	engine_data_str->MOVE->horizontal_angle += engine_data_str->MOVE->mouse_speed * (float)(engine_data_str->window_x / 2.0f - engine_data_str->MOVE->x_pos);
	engine_data_str->MOVE->vertical_angle += engine_data_str->MOVE->mouse_speed * (float)(engine_data_str->window_y / 2.0f - engine_data_str->MOVE->y_pos);
	if(engine_data_str->MOVE->vertical_angle > 1.4f){
		engine_data_str->MOVE->vertical_angle = 1.4f;
	}
	if(engine_data_str->MOVE->vertical_angle < -1.4f){
		engine_data_str->MOVE->vertical_angle = -1.4f;
	}
	
	//if(old_x_pos != engine_data_str->MOVE->x_pos || old_y_pos != engine_data_str->MOVE->y_pos){
	float look_vector[3] = {
		cos(engine_data_str->MOVE->vertical_angle) * sin(engine_data_str->MOVE->horizontal_angle),
		sin(engine_data_str->MOVE->vertical_angle),
		cos(engine_data_str->MOVE->vertical_angle) * cos(engine_data_str->MOVE->horizontal_angle)
	};
	glm_vec3_make(look_vector, engine_data_str->MOVE->direction_look);

	mat4 view;
	vec3 position_direction;
	glm_vec3_add(engine_data_str->MOVE->direction_look, engine_data_str->MOVE->position, position_direction);
	vec3 up = {0.0f, 1.0f, 0.0f};
	glm_lookat(engine_data_str->MOVE->position, position_direction, up, view);
	//}
}

void zenithra_update_position(struct InEngineData *engine_data_str){
	float look_vector[3] = {
		cos(engine_data_str->MOVE->vertical_angle) * sin(engine_data_str->MOVE->horizontal_angle),
		sin(engine_data_str->MOVE->vertical_angle),
		cos(engine_data_str->MOVE->vertical_angle) * cos(engine_data_str->MOVE->horizontal_angle)
	};
	glm_vec3_make(look_vector, engine_data_str->MOVE->direction_look);

	float strafe_vector[3] = {
		sin(engine_data_str->MOVE->horizontal_angle - 3.14f / 2.0f),
		0,
		cos(engine_data_str->MOVE->horizontal_angle - 3.14f / 2.0f)
	};
	glm_vec3_make(strafe_vector, engine_data_str->MOVE->direction_strafe);

	float vertical_vector[3] = {
		0,
		1,
		0
	};
	glm_vec3_make(vertical_vector, engine_data_str->MOVE->direction_vertical);
	
	vec3 delta_time, speed, dts, gravity;
	glm_vec3_make((vec3){engine_data_str->delta_time, engine_data_str->delta_time, engine_data_str->delta_time}, delta_time);
	glm_vec3_make((vec3){engine_data_str->MOVE->speed, engine_data_str->MOVE->speed, engine_data_str->MOVE->speed}, speed);
	glm_vec3_make((vec3){engine_data_str->MOVE->gravity, engine_data_str->MOVE->gravity, engine_data_str->MOVE->gravity}, gravity);

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if(keyState[SDL_SCANCODE_A]){
		glm_vec3_mul(engine_data_str->MOVE->direction_strafe, delta_time, dts);
		glm_vec3_mul(dts, speed, dts);
		glm_vec3_sub(engine_data_str->MOVE->position, dts, engine_data_str->MOVE->position);
	}
	if(keyState[SDL_SCANCODE_W]){
		glm_vec3_mul(engine_data_str->MOVE->direction_look, delta_time, dts);
		glm_vec3_mul(dts, speed, dts);
		glm_vec3_add(engine_data_str->MOVE->position, dts, engine_data_str->MOVE->position);
	}
	if(keyState[SDL_SCANCODE_S]){
		glm_vec3_mul(engine_data_str->MOVE->direction_look, delta_time, dts);
		glm_vec3_mul(dts, speed, dts);
		glm_vec3_sub(engine_data_str->MOVE->position, dts, engine_data_str->MOVE->position);
	}
	if(keyState[SDL_SCANCODE_D]){
		glm_vec3_mul(engine_data_str->MOVE->direction_strafe, delta_time, dts);
		glm_vec3_mul(dts, speed, dts);
		glm_vec3_add(engine_data_str->MOVE->position, dts, engine_data_str->MOVE->position);
	}
	if(keyState[SDL_SCANCODE_LCTRL]){
		glm_vec3_mul(engine_data_str->MOVE->direction_vertical, delta_time, dts);
		glm_vec3_mul(dts, speed, dts);
		glm_vec3_sub(engine_data_str->MOVE->position, dts, engine_data_str->MOVE->position);
	}
	if(keyState[SDL_SCANCODE_SPACE]){
		glm_vec3_mul(engine_data_str->MOVE->direction_vertical, delta_time, dts);
		glm_vec3_mul(dts, speed, dts);
		glm_vec3_add(engine_data_str->MOVE->position, dts, engine_data_str->MOVE->position);
	}
}