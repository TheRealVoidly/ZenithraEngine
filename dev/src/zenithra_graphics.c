#include"zenithra_core.h"

void zenithra_create_renderer_buffer(struct InEngineData *engine_data_str){
	int count = engine_data_str->renderer_x * engine_data_str->renderer_y;

	RENDERER_BUF *head = NULL;
	RENDERER_BUF *next_in_line = NULL;

	for(int i = 0; i < count; i++){
		RENDERER_BUF *node = malloc(sizeof *node);
		if(!node){
			char error_message[128];
			snprintf(error_message, sizeof error_message, "RENDERER_BUF allocation failed at pixel %d", i);
			zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, error_message);
		}

		node->x = i % engine_data_str->renderer_x;
		node->y = i / engine_data_str->renderer_x;

		node->r = 0;
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