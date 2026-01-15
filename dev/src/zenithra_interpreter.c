#include"zenithra_core.h"
#include"zenithra_debug.h"

void zenithra_register_callback(struct InEngineData *engine_data_str, char *callback_name, char *callback_request){

}

/**
 * This function handles commands outside of loop once on startup
 * 
 * @param engine_data_str
 * @param obj
**/

void zenithra_interpreter_begin(struct InEngineData *engine_data_str, struct ObjectData **obj){
	engine_data_str->INTERPRETER->command = malloc(sizeof(char) * 256);
	memset(engine_data_str->INTERPRETER->command, 0, 256);
	engine_data_str->INTERPRETER->offset = 0;

	engine_data_str->INTERPRETER->iv = zenithra_interpreter_create_variable_node();
	memset(engine_data_str->INTERPRETER->iv->variable_name, 0, 1024);

	zenithra_read_command(engine_data_str, NULL);
	while((strncmp(engine_data_str->INTERPRETER->command, "{", 1) != 0) && engine_data_str->INTERPRETER->fval != 0){		
		zenithra_interpreter_check_commands(engine_data_str, obj, NULL);

		zenithra_read_command(engine_data_str, NULL);
	}
	engine_data_str->INTERPRETER->offset = 0;
}

/**
 * This function handles called scripts
 * 
 * @param engine_data_str
 * @param obj
 * @param file_name = if NULL default to entry.zen
**/

void zenithra_interpreter_run_through(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
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

void zenithra_interpreter_loop(struct InEngineData *engine_data_str, struct ObjectData **obj){
	zenithra_read_command(engine_data_str, NULL);

	while(strncmp(engine_data_str->INTERPRETER->command, "{", 1) != 0){
		zenithra_read_command(engine_data_str, NULL);
	}

	while(strncmp(engine_data_str->INTERPRETER->command, "}", 1) != 0){
		zenithra_interpreter_check_commands(engine_data_str, obj, NULL);

		zenithra_read_command(engine_data_str, NULL);
	}
	engine_data_str->INTERPRETER->offset = 0;
}

/**
 * This function retrieves commands word by word
 * 
 * @param engine_data_str
 * @param file_name = If NULL default open entry.zen
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
 * @param file_name = Name of script, NULL == entry.zen
**/

void zenithra_interpreter_check_commands(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
	if(strncmp(engine_data_str->INTERPRETER->command, "#", 1) == 0){
		return;
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "register_variable") == 0){
		zenithra_interpreter_command_register_variable(engine_data_str, file_name);
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "load_object") == 0){
		zenithra_interpreter_command_load_object(engine_data_str, obj, file_name);
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "bind_texture_to_object") == 0){
		zenithra_interpreter_command_bind_texture_to_object(engine_data_str, obj, file_name);
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "move_object") == 0){
		zenithra_interpreter_command_move_object(engine_data_str, obj, file_name);
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "call_script") == 0){
		zenithra_interpreter_command_call_script(engine_data_str, obj, file_name);
	}

	if(strcmp(engine_data_str->INTERPRETER->command, "update_variable") == 0){
		zenithra_interpreter_update_variable(engine_data_str, file_name);
	}
}

/**
 * Matches input variable to variable list
 * 
 * @param engine_data_str
 * @param variable_name = input variable that we want to match
 * @return valid node containing matched variable name, otherwise if no variable was found return NULL
**/

struct InterpreterVariable* zenithra_interpreter_match_variable_name(struct InEngineData *engine_data_str, char *variable_name){
	struct InterpreterVariable *node = engine_data_str->INTERPRETER->iv;
	while(1){
		if(strcmp(node->variable_name, variable_name) == 0){
			return node;
		}
		if(!node->next){
			return NULL;
		}
		node = node->next;
	}
	return NULL;
}

void zenithra_interpreter_free_variable_list(struct InterpreterVariable **head){
    struct InterpreterVariable *current = *head;
    struct InterpreterVariable *next;
    int i = 0;

    while(current){
        next = current->next;
        zenithra_free((void**)&current);
        current = next;
    }

    *head = NULL;
}

struct InterpreterVariable* zenithra_interpreter_create_variable_node(){
    struct InterpreterVariable *node = malloc(sizeof(struct InterpreterVariable));
    if(!node){
        zenithra_log_err(__FILE__, __LINE__, "Node memory allocation failed");
        return NULL;
    }
    node->next = NULL;
    return node;
}