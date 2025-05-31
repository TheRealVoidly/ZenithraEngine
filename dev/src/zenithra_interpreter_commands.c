#include"zenithra_core.h"
#include"zenithra_debug.h"

void zenithra_interpreter_command_register_variable(struct InEngineData *engine_data_str, char *file_name){
	char name[256], value[256];

	zenithra_read_command(engine_data_str, file_name);
	strcpy(name, engine_data_str->INTERPRETER->command);

	zenithra_read_command(engine_data_str, file_name);
	strcpy(value, engine_data_str->INTERPRETER->command);

	struct InterpreterVariable *node = engine_data_str->INTERPRETER->iv;
	while(node->next){
		node = node->next;
	}
	node->next = zenithra_interpreter_create_variable_node();
	strcpy(node->next->variable_name, name);

	node->next->f_value = atof(value);
}

void zenithra_interpreter_command_load_object(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
	char object_path[256], texture_path[256], temp[256], variable_name[256];

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
			strcpy(variable_name, engine_data_str->INTERPRETER->command);

			struct InterpreterVariable *node = zenithra_interpreter_match_variable_name(engine_data_str, variable_name);
			node->f_value = engine_data_str->obj_num - 1;
		}
	}
}

void zenithra_interpreter_command_bind_texture_to_object(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
	char variable_name[256], texture_path[256];

	zenithra_read_command(engine_data_str, file_name);
	strcpy(variable_name, engine_data_str->INTERPRETER->command);

	zenithra_read_command(engine_data_str, file_name);
	strcpy(texture_path, engine_data_str->INTERPRETER->command);

	struct InterpreterVariable *node = zenithra_interpreter_match_variable_name(engine_data_str, variable_name);
	if(!node){
		zenithra_rebind_texture(obj, atof(variable_name), texture_path);
	}else{
		zenithra_rebind_texture(obj, node->f_value, texture_path);
	}
}

void zenithra_interpreter_command_move_object(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
	char variable_name[256], vector[256], value[256];

	zenithra_read_command(engine_data_str, file_name);
	strcpy(variable_name, engine_data_str->INTERPRETER->command);

	zenithra_read_command(engine_data_str, file_name);
	strcpy(vector, engine_data_str->INTERPRETER->command);

	zenithra_read_command(engine_data_str, file_name);
	strcpy(value, engine_data_str->INTERPRETER->command);

	struct InterpreterVariable *node_value = zenithra_interpreter_match_variable_name(engine_data_str, value);
	struct InterpreterVariable *node_name = zenithra_interpreter_match_variable_name(engine_data_str, variable_name);
	if(node_value){
		if(!node_name){
			zenithra_move_object(engine_data_str, obj, atof(variable_name), vector[0], node_value->f_value);
		}else{
			zenithra_move_object(engine_data_str, obj, node_name->f_value, vector[0], node_value->f_value);
		}
	}else{
		if(!node_name){
			zenithra_move_object(engine_data_str, obj, atof(variable_name), vector[0], atof(value));
		}else{
			zenithra_move_object(engine_data_str, obj, node_name->f_value, vector[0], atof(value));
		}
	}
}

void zenithra_interpreter_command_call_script(struct InEngineData *engine_data_str, struct ObjectData **obj, char *file_name){
	char script_path[256];

	zenithra_read_command(engine_data_str, file_name);
	strcpy(script_path, engine_data_str->INTERPRETER->command);

	int old_offset = engine_data_str->INTERPRETER->offset;
	zenithra_interpreter_run_through(engine_data_str, obj, script_path);
	engine_data_str->INTERPRETER->offset = old_offset;
}

void zenithra_interpreter_update_variable(struct InEngineData *engine_data_str, char *file_name){
	char variable1[256], variable2[256], operand[256], variable3[256];
	struct InterpreterVariable *node1, *node2, *node3;
	bool v2_is_number = false, v3_is_number = false;
	int variant = 0;

	zenithra_read_command(engine_data_str, file_name);
	strcpy(variable1, engine_data_str->INTERPRETER->command);
	node1 = zenithra_interpreter_match_variable_name(engine_data_str, variable1);
	if(!node1){
		return;
	}

	zenithra_read_command(engine_data_str, file_name);

	zenithra_read_command(engine_data_str, file_name);
	strcpy(variable2, engine_data_str->INTERPRETER->command);
	if(atof(variable2) == 0){
		node2 = zenithra_interpreter_match_variable_name(engine_data_str, variable2);
		if(!node2){
			return;
		}
	}else{
		v2_is_number = true;
	}

	zenithra_read_command(engine_data_str, file_name);
	strcpy(operand, engine_data_str->INTERPRETER->command);

	zenithra_read_command(engine_data_str, file_name);
	strcpy(variable3, engine_data_str->INTERPRETER->command);
	if(atof(variable3) == 0){
		node3 = zenithra_interpreter_match_variable_name(engine_data_str, variable3);
		if(!node3){
			return;
		}
	}else{
		v3_is_number = true;
	}

	if(v2_is_number){
		if(v3_is_number){
			variant = 0;
		}else{
			variant = 1;
		}
	}else{
		if(v3_is_number){
			variant = 2;
		}else{
			variant = 3;
		}
	}

	switch(variant){
	case 0:
		switch(operand[0]){
		case '+':
			node1->f_value = atof(variable2) + atof(variable3);
			break;
		case '-':
			node1->f_value = atof(variable2) - atof(variable3);
			break;
		case '*':
			node1->f_value = atof(variable2) * atof(variable3);
			break;
		case '/':
			node1->f_value = atof(variable2) / atof(variable3);
			break;
		default:
			break;
		}
		break;
	case 1:
		switch(operand[0]){
		case '+':
			node1->f_value = atof(variable2) + node3->f_value;
			break;
		case '-':
			node1->f_value = atof(variable2) - node3->f_value;
			break;
		case '*':
			node1->f_value = atof(variable2) * node3->f_value;
			break;
		case '/':
			node1->f_value = atof(variable2) / node3->f_value;
			break;
		default:
			break;
		}
		break;
	case 2:
		switch(operand[0]){
		case '+':
			node1->f_value = node2->f_value + atof(variable3);
			break;
		case '-':
			node1->f_value = node2->f_value - atof(variable3);
			break;
		case '*':
			node1->f_value = node2->f_value * atof(variable3);
			break;
		case '/':
			node1->f_value = node2->f_value / atof(variable3);
			break;
		default:
			break;
		}
		break;
	case 3:
		switch(operand[0]){
		case '+':
			node1->f_value = node2->f_value + node3->f_value;
			break;
		case '-':
			node1->f_value = node2->f_value - node3->f_value;
			break;
		case '*':
			node1->f_value = node2->f_value * node3->f_value;
			break;
		case '/':
			node1->f_value = node2->f_value / node3->f_value;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}