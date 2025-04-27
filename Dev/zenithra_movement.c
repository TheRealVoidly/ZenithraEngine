#include"zenithra_core.h"

void Zenithra_InitMovementVals(struct in_engine_data *engineDataStr){
	glm_vec3_make((vec3){4, 0, 4}, engineDataStr->MOVE->position);
	engineDataStr->MOVE->horizontalAngle = 3.14f;
	engineDataStr->MOVE->verticalAngle = 0.0f;
	engineDataStr->MOVE->FOV = 90.0f;
	engineDataStr->MOVE->speed = 0.005f;
	engineDataStr->MOVE->gravity = 0.0000001f;
	engineDataStr->MOVE->mouseSpeed = 0.001f;
	engineDataStr->MOVE->fly = false;
}

void Zenithra_CalcMouseMovement(struct in_engine_data *engineDataStr){
	int xPos, yPos;
	SDL_GetMouseState(&xPos, &yPos);
	SDL_WarpMouseInWindow(engineDataStr->SDL->window, engineDataStr->window_x / 2, engineDataStr->window_y / 2);

	engineDataStr->MOVE->horizontalAngle += engineDataStr->MOVE->mouseSpeed * (float)(engineDataStr->window_x / 2.0f - xPos);
	engineDataStr->MOVE->verticalAngle += engineDataStr->MOVE->mouseSpeed * (float)(engineDataStr->window_y / 2.0f - yPos);
}

void Zenithra_UpdatePosition(struct in_engine_data *engineDataStr){
	vec3 deltaTime, speed, dts, gravity;

	float walkVector[3] = {
		cos(engineDataStr->MOVE->verticalAngle) * sin(engineDataStr->MOVE->horizontalAngle),
		sin(engineDataStr->MOVE->verticalAngle),
		cos(engineDataStr->MOVE->verticalAngle) * cos(engineDataStr->MOVE->horizontalAngle)
	};
	glm_vec3_make(walkVector, engineDataStr->MOVE->directionWalk);

	float strafeVector[3] = {
		sin(engineDataStr->MOVE->horizontalAngle - 3.14f / 2.0f),
		0,
		cos(engineDataStr->MOVE->horizontalAngle - 3.14f / 2.0f)
	};
	glm_vec3_make(strafeVector, engineDataStr->MOVE->directionStrafe);

	float verticalVector[3] = {
		0,
		1,
		0
	};
	glm_vec3_make(verticalVector, engineDataStr->MOVE->directionVertical);
	
	glm_vec3_make((vec3){engineDataStr->deltaTime, engineDataStr->deltaTime, engineDataStr->deltaTime}, deltaTime);
	glm_vec3_make((vec3){engineDataStr->MOVE->speed, engineDataStr->MOVE->speed, engineDataStr->MOVE->speed}, speed);
	glm_vec3_make((vec3){engineDataStr->MOVE->gravity, engineDataStr->MOVE->gravity, engineDataStr->MOVE->gravity}, gravity);

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if(engineDataStr->MOVE->fly){
		if(keyState[SDL_SCANCODE_A]){
			glm_vec3_mul(engineDataStr->MOVE->directionStrafe, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_sub(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_W]){
			glm_vec3_mul(engineDataStr->MOVE->directionWalk, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_add(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_S]){
			glm_vec3_mul(engineDataStr->MOVE->directionWalk, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_sub(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_D]){
			glm_vec3_mul(engineDataStr->MOVE->directionStrafe, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_add(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_LCTRL]){
			glm_vec3_mul(engineDataStr->MOVE->directionVertical, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_sub(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_SPACE]){
			glm_vec3_mul(engineDataStr->MOVE->directionVertical, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_add(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
	}else{
		if(keyState[SDL_SCANCODE_A]){
			glm_vec3_mul(engineDataStr->MOVE->directionStrafe, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_sub(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_W]){
			glm_vec3_mul(engineDataStr->MOVE->directionWalk, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_add(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_S]){
			glm_vec3_mul(engineDataStr->MOVE->directionWalk, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_sub(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_D]){
			glm_vec3_mul(engineDataStr->MOVE->directionStrafe, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_add(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_LCTRL]){
			glm_vec3_mul(engineDataStr->MOVE->directionVertical, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_sub(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
		if(keyState[SDL_SCANCODE_SPACE]){
			glm_vec3_mul(engineDataStr->MOVE->directionVertical, deltaTime, dts);
			glm_vec3_mul(dts, speed, dts);
			glm_vec3_add(engineDataStr->MOVE->position, dts, engineDataStr->MOVE->position);
		}
	}
}