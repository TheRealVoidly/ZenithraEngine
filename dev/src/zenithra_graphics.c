#include"zenithra_core.h"

/**
 * Creates the renderer buffer
 * Stores pixel coordinate and color of each pixel alongside the lines and triangles
**/

void zenithra_create_renderer_buffer(struct InEngineData *engine_data_str){
	int count = engine_data_str->renderer_X * engine_data_str->renderer_Y;

	RENDERER_BUF *head = NULL;
	RENDERER_BUF *next_in_line = NULL;

	for(int i = 0; i < count; i++){
		RENDERER_BUF *node = malloc(sizeof *node);
		if(!node){
			char error_message[128];
			snprintf(error_message, sizeof error_message, "RENDERER_BUF allocation failed at pixel %d", i);
			zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, error_message);
		}

		node->points = malloc(sizeof *node->points);
		node->points->x = i % engine_data_str->renderer_X; //Pixel #X left -> right
		node->points->y = i / engine_data_str->renderer_X; //Pixel #Y up -> down

		node->r = 0; //Initialize all pixels as black
		node->g = 0;
		node->b = 0;
		node->w = 255;

		node->next = NULL;

		if(!head){
			head = node;
		}else{
			next_in_line->next = node;
		}

		next_in_line = node;
	}
	engine_data_str->RENDERER_BUF = head;
	
	zenithra_log_msg("Renderer buffer created successfully");
}

/**
 * Check if point is renderable and should be rendered and render it
 * 
 * @param Xw, Yw, Zw = World coordinates of the point
**/

void zenithra_check_and_save_for_rendering(struct InEngineData *engine_data_str, double Xw, double Yw, double Zw){
	double Xr = Xw - engine_data_str->MOVE->cam_X; //Point coordinates relative to camera
	double Yr = Yw - engine_data_str->MOVE->cam_Y;
	double Zr = Zw - engine_data_str->MOVE->cam_Z;

    double yaw_rad = engine_data_str->MOVE->cam_yaw * M_PI / 180.0; //Convert yaw angle to rad
    double pitch_rad = engine_data_str->MOVE->cam_pitch * M_PI / 180.0; //Convert pitch angle to rad

	double X_cam = Xr * cos(yaw_rad) - Zr * sin(yaw_rad); //Yaw rotation around Y-axis
	double Y_cam = Yr;
	double Z_cam = Xr * sin(yaw_rad) + Zr * cos(yaw_rad);

	double Y_cam2 = Y_cam * cos(pitch_rad) - Z_cam * sin(pitch_rad); //Pitch rotation around X-axis
	double Z_cam2 = Y_cam * sin(pitch_rad) + Z_cam * cos(pitch_rad);
	Y_cam = Y_cam2;
	Z_cam = Z_cam2;

	if(Z_cam < engine_data_str->MOVE->Z_near){
        //Point is between camerra and the near plane
		return;
	}
	if(Z_cam > engine_data_str->MOVE->Z_far){
        //Point is behind the far plane
		return;
	}

    double max_X_at_Z = Z_cam / tan((90.0 - engine_data_str->MOVE->hFOV / 2.0) * M_PI / 180.0); //Find maximum visible X at Z_cam
    double max_Y_at_Z = Z_cam / tan((90.0 - engine_data_str->MOVE->vFOV / 2.0) * M_PI / 180.0); //Find maximum visible Y at Z_cam

    if(X_cam < -max_X_at_Z || X_cam > max_X_at_Z || Y_cam < -max_Y_at_Z || Y_cam > max_Y_at_Z){
        // Point is outside FOV
    	return;
    }

	double norm_X = (X_cam / max_X_at_Z + 1.0) / 2.0; //Normalize X and Y to [0,1]
	double norm_Y = (Y_cam / max_Y_at_Z + 1.0) / 2.0;
    int screen_X = (int)round(norm_X * engine_data_str->renderer_X); //Map to screen pixels
    int screen_Y = (int)round(norm_Y * engine_data_str->renderer_Y);

    RENDERER_BUF *node = engine_data_str->RENDERER_BUF;
    while(node){
    	if(screen_X == node->points->x && screen_Y == node->points->y){
    		node->r = 255;
    	}

    	node = node->next;
    }
}

/**
 * Draws all the pixels stored in the engine_data_str->RENDERER_BUF
**/

void zenithra_draw_points(struct InEngineData *engine_data_str){
	RENDERER_BUF *node = engine_data_str->RENDERER_BUF;
	while(node){
		SDL_SetRenderDrawColor(engine_data_str->SDL->renderer, node->r, node->g, node->b, node->w);
		SDL_RenderDrawPoint(engine_data_str->SDL->renderer, node->points->x, node->points->y);

		node = node->next;
	}
}

/**
 * Draws all lines that are stored in engine_data_str->RENDERER_BUF
**/

void zenithra_draw_lines(struct InEngineData *engine_data_str){
	RENDERER_BUF *node = engine_data_str->RENDERER_BUF;
	while(node){
		SDL_SetRenderDrawColor(engine_data_str->SDL->renderer, node->r, node->g, node->b, node->w);
		SDL_RenderDrawLine(engine_data_str->SDL->renderer, node->points->x, node->points->y, );

		node = node->next;
	}
}