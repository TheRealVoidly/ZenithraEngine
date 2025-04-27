#include"zenithra_core.h"

bool Zenithra_HandleEventPoll(struct in_engine_data *engineDataStr){
	SDL_Event event;
	bool programShouldQuit = false;
	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	while(SDL_PollEvent(&event) != 0){
		if(event.type == SDL_QUIT){
			programShouldQuit = true;
		}
		if(keyState[SDL_SCANCODE_ESCAPE] && !engineDataStr->SDL->focusLost && !engineDataStr->KEYS->escape){
			SDL_ShowCursor(SDL_ENABLE);
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_SetWindowGrab(engineDataStr->SDL->window, SDL_FALSE);
			engineDataStr->SDL->focusLost = true;
			engineDataStr->KEYS->escape = true;
		}
		if(keyState[SDL_SCANCODE_ESCAPE] && engineDataStr->SDL->focusLost && !engineDataStr->KEYS->escape){
			SDL_ShowCursor(SDL_DISABLE);
			SDL_SetRelativeMouseMode(SDL_TRUE);
			SDL_SetWindowGrab(engineDataStr->SDL->window, SDL_TRUE);
			engineDataStr->SDL->focusLost = false;
			engineDataStr->KEYS->escape = true;
		}
		if(keyState[SDL_SCANCODE_RSHIFT] && !engineDataStr->MOVE->fly && !engineDataStr->KEYS->rShift){
			engineDataStr->MOVE->fly = true;
			engineDataStr->KEYS->rShift = true;
		}
		if(keyState[SDL_SCANCODE_RSHIFT] && engineDataStr->MOVE->fly && !engineDataStr->KEYS->rShift){
			engineDataStr->MOVE->fly = false;
			engineDataStr->KEYS->rShift = true;
		}

		if(engineDataStr->KEYS->escape){
			if(!keyState[SDL_SCANCODE_ESCAPE]){
				engineDataStr->KEYS->escape = false;
			}
		}
		if(engineDataStr->KEYS->rShift){
			if(!keyState[SDL_SCANCODE_RSHIFT]){
				engineDataStr->KEYS->rShift = false;
			}
		}
	}
	return programShouldQuit;
}