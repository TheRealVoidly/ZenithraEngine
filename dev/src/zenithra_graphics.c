#include"zenithra_core.h"
#include"vertex_shader.h"
#include"fragment_shader.h"

GLuint zenithra_load_shaders(struct InEngineData *engine_data_str){
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;

	glShaderSource(vertex_shader_id, 1, vertex_shader_source, NULL);
	glCompileShader(vertex_shader_id);

	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
	if(!result){
		zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "Error compiling vertex shader");
	}

	glShaderSource(fragment_shader_id, 1, fragment_shader_source, NULL);
	glCompileShader(fragment_shader_id);

	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
	if(!result){
		zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "Error compiling fragment shader");
	}

	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader_id);
	glAttachShader(program_id, fragment_shader_id);
	glLinkProgram(program_id);

	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	if(!result){
		zenithra_critical_error_occured(engine_data_str, __FILE__, __LINE__, "Error linking program");
	}

	glDetachShader(program_id, vertex_shader_id);
	glDetachShader(program_id, fragment_shader_id);

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	return program_id;
}

struct ObjectData* zenithra_load_obj(struct InEngineData *engine_data_str, bool engine_obj, const char* object_file_name, const char* texture_file_name){
	FILE *fp = NULL;
	char buffer[255];
	int i = 0, res;
	long long triangles = 0;

	struct ObjectData *obj = (void*)malloc(sizeof(struct ObjectData));

	GLfloat *uvs_buffer_data, *normals_buffer_data, *temp_vertex_buffer_data, *temp_normals_buffer_data, *temp_uvs_buffer_data;

	fp = fopen(object_file_name, "r");
	if(!fp){
		zenithra_log_err(__FILE__, __LINE__, "OBJ file could not be opened");
		zenithra_log_msg(object_file_name);
		return NULL;
	}

	while(1){
		res = fscanf(fp, "%s", buffer);

		if(res == EOF){
			break;
		}

		if(strcmp(buffer, "f") == 0){
			triangles++;
		}
	}

	obj->obj_size = triangles * 3;

	int *face_buffer_data;
	face_buffer_data = (int*)malloc(sizeof(int) * (obj->obj_size * 3));

	fseek(fp, 0, SEEK_SET);
	while(1){
		res = fscanf(fp, "%s", buffer);

		if(res == EOF){
			break;
		}

		if(strcmp(buffer, "f") == 0){
			fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d", &face_buffer_data[i], &face_buffer_data[i+1], &face_buffer_data[i+2], &face_buffer_data[i+3], &face_buffer_data[i+4], &face_buffer_data[i+5], &face_buffer_data[i+6], &face_buffer_data[i+7], &face_buffer_data[i+8]);
			i += 9;
		}
	}

	obj->vertex_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->obj_size * 3));
	uvs_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->obj_size * 2));
	normals_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->obj_size * 3));
	temp_vertex_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->obj_size * 3));
	temp_uvs_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->obj_size * 2));
	temp_normals_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->obj_size * 3));

	fseek(fp, 0, SEEK_SET);
	res = fscanf(fp, "%s", buffer);
	int a = 0, b = 0, c = 0;
	while(res != EOF){
		res = fscanf(fp, "%s", buffer);
		if(strcmp(buffer, "v") == 0){
			fscanf(fp, "%f %f %f", &temp_vertex_buffer_data[a], &temp_vertex_buffer_data[a+1], &temp_vertex_buffer_data[a+2]);
			a += 3;
		}
		if(strcmp(buffer, "vn") == 0){
			fscanf(fp, "%f %f %f", &temp_normals_buffer_data[b], &temp_normals_buffer_data[b+1], &temp_normals_buffer_data[b+2]);
			b += 3;
		}
		if(strcmp(buffer, "vt") == 0){
			fscanf(fp, "%f %f", &temp_uvs_buffer_data[c], &temp_uvs_buffer_data[c+1]);
			c += 2;
		}
	}
	fclose(fp);

	int vi, ti, ni, uv_i;
	for (i = 0; i < obj->obj_size * 3; i += 3) {
		vi = face_buffer_data[i] - 1;
		ti = face_buffer_data[i+1] - 1;
		ni = face_buffer_data[i+2] - 1;

		obj->vertex_buffer_data[i] = temp_vertex_buffer_data[vi*3];
		obj->vertex_buffer_data[i+1] = temp_vertex_buffer_data[vi*3+1];
		obj->vertex_buffer_data[i+2] = temp_vertex_buffer_data[vi*3+2];

		normals_buffer_data[i] = temp_normals_buffer_data[ni*3];
		normals_buffer_data[i+1] = temp_normals_buffer_data[ni*3+1];
		normals_buffer_data[i+2] = temp_normals_buffer_data[ni*3+2];

		uv_i = (i / 3) * 2;
		uvs_buffer_data[uv_i] = temp_uvs_buffer_data[ti*2];
		uvs_buffer_data[uv_i+1] = temp_uvs_buffer_data[ti*2+1];
	}

	glGenBuffers(1, &obj->obj_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->obj_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj->obj_size * 3, &obj->vertex_buffer_data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->obj_uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->obj_uv_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj->obj_size * 2, &uvs_buffer_data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->obj_normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->obj_normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj->obj_size * 3, &normals_buffer_data[0], GL_STATIC_DRAW);

	zenithra_free((void**)&uvs_buffer_data);
	zenithra_free((void**)&normals_buffer_data);
	zenithra_free((void**)&face_buffer_data);
	zenithra_free((void**)&temp_vertex_buffer_data);
	zenithra_free((void**)&temp_normals_buffer_data);
	zenithra_free((void**)&temp_uvs_buffer_data);

	obj->obj_bound_texture = zenithra_create_texture(texture_file_name);

	obj->engine_obj = engine_obj;
	engine_data_str->obj_num++;
	return obj;
}

void zenithra_rebind_texture(struct ObjectData *obj, const char* file_name){
	obj->obj_bound_texture = zenithra_create_texture(file_name);
}

GLuint zenithra_create_texture(const char* file_name){
	char file_check[4];
	unsigned char* data;
	unsigned char header[124];
	FILE *fp = NULL;

	fp = fopen(file_name, "rb");
	if(!fp){
		zenithra_log_err(__FILE__, __LINE__, "BMP file not found");
		zenithra_log_msg(file_name);
		return 0;
	}

	fread(file_check, 1, 4, fp);
	if(strncmp(file_check, "DDS ", 4) != 0){
		zenithra_log_err(__FILE__, __LINE__, "Invalid texture file");
		zenithra_log_msg(file_name);
		fclose(fp);
		return 0;
	}

	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linear_size = *(unsigned int*)&(header[16]);
	unsigned int mip_map_count = *(unsigned int*)&(header[24]);
	unsigned int buf_size;

	fseek(fp, 0, SEEK_END);
	buf_size = ftell(fp) - 128;
	fseek(fp, 128, SEEK_SET);
	data = (unsigned char*)malloc(buf_size);
	int bytesRead = fread(data, 1, buf_size, fp);
	if(bytesRead != buf_size){
		zenithra_log_err(__FILE__, __LINE__, "Failed to read DDS texture data");
		zenithra_free((void*)&data);
		fclose(fp);
		return 0;
	}
	fclose(fp);

	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	unsigned int format;

	char fourCC[4];
	memcpy(fourCC, &header[80], 4);

	if(strncmp(fourCC, "DXT1", 4) == 0){
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	}
	else if(strncmp(fourCC, "DXT3", 4) == 0){
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	}
	else if(strncmp(fourCC, "DXT5", 4) == 0){
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	}
	else{
		zenithra_log_err(__FILE__, __LINE__, "Unsupported DDS format");
		zenithra_free((void*)&data);
		return 0;
	}

	if(strncmp(fourCC, "DX10", 4) == 0){
		zenithra_log_err(__FILE__, __LINE__, "DX10 DDS files not supported");
		zenithra_free((void*)&data);
		return 0;
	}

	unsigned int block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;
	bool use_mipmaps = mip_map_count > 1;

	unsigned int levels = use_mipmaps ? mip_map_count : 1;
	for (unsigned int level = 0; level < levels; ++level) {
		unsigned int w = width > 0 ? width : 1;
		unsigned int h = height > 0 ? height : 1;
		unsigned int size = ((w + 3) / 4) * ((h + 3) / 4) * block_size;

		if(offset + size > buf_size){
			zenithra_log_err(__FILE__, __LINE__, "DDS mipmap data overflows buffer");
			zenithra_free((void*)&data);
			return 0;
		}

		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, w, h, 0, size, data + offset);
		offset += size;

		if(use_mipmaps){
			width = width > 1 ? width / 2 : 1;
			height = height > 1 ? height / 2 : 1;
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, use_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return texture_id;
}

void zenithra_render_object(struct InEngineData *engine_data_str, struct ObjectData **obj, int obj_num){
	mat4 projection, view, mvp;

	if(obj[obj_num] == NULL){
		return;
	}

	glm_perspective(glm_rad(engine_data_str->MOVE->fov), (float) engine_data_str->window_x / (float) engine_data_str->window_y, 1.0f, 1000.0f, projection);

	vec3 position_direction;
	glm_vec3_add(engine_data_str->MOVE->direction_look, engine_data_str->MOVE->position, position_direction);
	vec3 up = {0.0f, 1.0f, 0.0f};
	glm_lookat(engine_data_str->MOVE->position, position_direction, up, view);

	glm_mat4_mul(projection, view, mvp);

    //GLint lightPosLoc = glGetUniformLocation(engine_data_str->GL->programID, "LightPosition_worldspace");
    //glUniform3f(lightPosLoc, 0.0f, 5.0f, 0.0f);

	glUniformMatrix4fv(engine_data_str->GL->matrix_id, 1, GL_FALSE, &mvp[0][0]);

	glBindTexture(GL_TEXTURE_2D, obj[obj_num]->obj_bound_texture);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, obj[obj_num]->obj_vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, obj[obj_num]->obj_uv_buffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, obj[obj_num]->obj_normal_buffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, obj[obj_num]->obj_size);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, 0);
}