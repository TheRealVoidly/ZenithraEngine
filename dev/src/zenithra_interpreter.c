#include"zenithra_core.h"
#include"zenithra_debug.h"

void zenithra_register_callback(){

}

/**
 * This function handles commands outside of loop once on startup
 * 
 * @param engine_data_str
 * @param obj
 * @param file_name = if NULL default to entry.zen
**/

void zenithra_interpreter_begin(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
	if(!file_name){
		engine_data_str->INTERPRETER->command = (char*)malloc(sizeof(char) * 256);
		memset(engine_data_str->INTERPRETER->command, 0, 256);
	}
	engine_data_str->INTERPRETER->offset = 0;

	zenithra_read_command(engine_data_str, file_name);
	while((strncmp(engine_data_str->INTERPRETER->command, "{", 1) != 0) && engine_data_str->INTERPRETER->fval != 0){		
		zenithra_interpreter_check_commands(engine_data_str, obj, file_name);

		zenithra_read_command(engine_data_str, file_name);
	}
	engine_data_str->INTERPRETER->offset = 0;
}

/**
 * This function handles commands in a loop in a script file
 * 
 * @param engine_data_str
 * @param obj
**/

void zenithra_interpreter_loop(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
	zenithra_read_command(engine_data_str, file_name);

	while(strncmp(engine_data_str->INTERPRETER->command, "{", 1) != 0){
		zenithra_read_command(engine_data_str, file_name);
	}

	while(strncmp(engine_data_str->INTERPRETER->command, "}", 1) != 0){
		zenithra_interpreter_check_commands(engine_data_str, obj, file_name);

		zenithra_read_command(engine_data_str, file_name);
	}
	engine_data_str->INTERPRETER->offset = 0;
}

/**
 * This function retrieves commands word by word
 * 
 * @param engine_data_str
**/

void zenithra_read_command(struct InEngineData *engine_data_str, char *file_name){
	memset(engine_data_str->INTERPRETER->command, 0, 256);
	int fd;
	if(!file_name){
		fd = open("./gamedata/scripts/entry.zen", O_RDONLY, 0644);
	}else{
		fd = open(file_name, O_RDONLY, 0644);
	}
	char c;
	int n = 0;

	while(1){
		engine_data_str->INTERPRETER->fval = pread(fd, &c, 1, engine_data_str->INTERPRETER->offset);
		engine_data_str->INTERPRETER->offset++;
		if(engine_data_str->INTERPRETER->fval == 0 || c == '\n' || c == ' '){
			close(fd);
			return;
		}

		engine_data_str->INTERPRETER->command[n] = c;
		n++;
	}
}

/**
 * This function checks through a list of known commands
 * 
 * @param engine_data_str
 * @param obj
**/

void zenithra_interpreter_check_commands(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
	if(strncmp(engine_data_str->INTERPRETER->command, "#", 1) == 0){
		return;
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "register_variable") == 0){
		char type[256], name[256], value[256];

		zenithra_read_command(engine_data_str, file_name);
		strcpy(type, engine_data_str->INTERPRETER->command);

		zenithra_read_command(engine_data_str, file_name);
		strcpy(name, engine_data_str->INTERPRETER->command);

		zenithra_read_command(engine_data_str, file_name);
		strcpy(value, engine_data_str->INTERPRETER->command);

		uint64_t variable_name = zenithra_8_byte_to_int(name);
		engine_data_str->INTERPRETER->iv[variable_name] = (struct InterpreterVariable*)malloc(sizeof(struct InterpreterVariable));

		if(strcmp(type, "int") == 0){
			engine_data_str->INTERPRETER->iv[variable_name]->i_value = atoi(value);
		}
		if(strcmp(type, "float") == 0){
			engine_data_str->INTERPRETER->iv[variable_name]->i_value = atof(value);
		}
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "load_object") == 0){
		char object_path[256], texture_path[256], temp[256], variable[256];

		zenithra_read_command(engine_data_str, file_name);
		strcpy(object_path, engine_data_str->INTERPRETER->command);

		zenithra_read_command(engine_data_str, file_name);
		strcpy(texture_path, engine_data_str->INTERPRETER->command);

		if(zenithra_alloc_new_obj(engine_data_str, obj)){
			obj[engine_data_str->obj_num] = zenithra_load_obj(engine_data_str, false, object_path, texture_path);

			zenithra_read_command(engine_data_str, file_name);
			strcpy(temp, engine_data_str->INTERPRETER->command);
			if(strcmp(temp, "->") == 0){
				zenithra_read_command(engine_data_str, file_name);
				strcpy(variable, engine_data_str->INTERPRETER->command);

				uint64_t variable_name = zenithra_8_byte_to_int(variable);
				engine_data_str->INTERPRETER->iv[variable_name]->i_value = engine_data_str->obj_num - 1;
			}
		}
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "bind_texture_to_object") == 0){
		char object_number[256], texture_path[256];

		zenithra_read_command(engine_data_str, file_name);
		strcpy(object_number, engine_data_str->INTERPRETER->command);

		zenithra_read_command(engine_data_str, file_name);
		strcpy(texture_path, engine_data_str->INTERPRETER->command);

		uint64_t variable_name = zenithra_8_byte_to_int(object_number);
		zenithra_rebind_texture(obj, engine_data_str->INTERPRETER->iv[variable_name]->i_value, texture_path);
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "move_object") == 0){
		char object_number[256], vector[256], value[256];

		zenithra_read_command(engine_data_str, file_name);
		strcpy(object_number, engine_data_str->INTERPRETER->command);

		zenithra_read_command(engine_data_str, file_name);
		strcpy(vector, engine_data_str->INTERPRETER->command);

		zenithra_read_command(engine_data_str, file_name);
		strcpy(value, engine_data_str->INTERPRETER->command);

		uint64_t variable_name = zenithra_8_byte_to_int(object_number);
		zenithra_move_object(engine_data_str, obj, engine_data_str->INTERPRETER->iv[variable_name]->i_value, vector[0], atof(value));
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "call_script") == 0){
		char script_path[256];

		zenithra_read_command(engine_data_str, file_name);
		strcpy(script_path, engine_data_str->INTERPRETER->command);

		int old_offset = engine_data_str->INTERPRETER->offset;
		zenithra_interpreter_begin(engine_data_str, obj, script_path);
		engine_data_str->INTERPRETER->offset = old_offset;
	}
}