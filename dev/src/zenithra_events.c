#include"zenithra_core.h"

bool zenithra_handle_event_poll(struct InEngineData *engine_data_str){
	SDL_Event event;
	bool program_should_quit = false;
	const Uint8* key_state = SDL_GetKeyboardState(NULL);
	while(SDL_PollEvent(&event) != 0){
		if(event.type == SDL_QUIT){
			program_should_quit = true;
		}
		if(key_state[SDL_SCANCODE_ESCAPE] && !engine_data_str->SDL->focus_lost && !engine_data_str->KEYS->escape){
			SDL_ShowCursor(SDL_ENABLE);
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_SetWindowGrab(engine_data_str->SDL->window, SDL_FALSE);
			engine_data_str->SDL->focus_lost = true;
			engine_data_str->KEYS->escape = true;
		}
		if(key_state[SDL_SCANCODE_ESCAPE] && engine_data_str->SDL->focus_lost && !engine_data_str->KEYS->escape){
			SDL_ShowCursor(SDL_DISABLE);
			SDL_SetRelativeMouseMode(SDL_TRUE);
			SDL_SetWindowGrab(engine_data_str->SDL->window, SDL_TRUE);
			engine_data_str->SDL->focus_lost = false;
			engine_data_str->KEYS->escape = true;
		}

		if(engine_data_str->KEYS->escape){
			if(!key_state[SDL_SCANCODE_ESCAPE]){
				engine_data_str->KEYS->escape = false;
			}
		}
	}
	return program_should_quit;
}