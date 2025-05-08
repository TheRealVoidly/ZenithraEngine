#include"zenithra_core.h"
#include"vertex_shader.h"
#include"fragment_shader.h"

GLuint Zenithra_LoadShaders(struct in_engine_data *engineDataStr){
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;

	glShaderSource(vertexShaderID, 1, vertexShaderSource, NULL);
	glCompileShader(vertexShaderID);

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	if(!result){
		Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, "Error compiling vertex shader");
	}

	glShaderSource(fragmentShaderID, 1, fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	if(!result){
		Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, "Error compiling fragment shader");
	}

	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	if(!result){
		Zenithra_CriticalErrorOccured(engineDataStr, __FILE__, __LINE__, "Error linking program");
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return programID;
}

struct object_data* Zenithra_LoadOBJ(struct in_engine_data *engineDataStr, const char* fileName){
	FILE *fp = NULL;
	char buffer[255];
	long triangles = 0;
	int res;

	struct list_temp_vec3 *head_vert = Zenithra_CreateNode((void*)&head_vert, true, sizeof(struct list_temp_vec3));
	struct list_temp_vec3 *node_vert = head_vert;

	struct list_temp_vec3 *head_norm = Zenithra_CreateNode((void*)&head_norm, true, sizeof(struct list_temp_vec3));
	struct list_temp_vec3 *node_norm = head_norm;

	struct list_temp_vec2 *head_text = Zenithra_CreateNode((void*)&head_text, true, sizeof(struct list_temp_vec2));
	struct list_temp_vec2 *node_text = head_text;

	struct list_temp_mat3 *head_face = Zenithra_CreateNode((void*)&head_face, true, sizeof(struct list_temp_mat3));
	struct list_temp_mat3 *node_face = head_face;

	fp = fopen(fileName, "r");
	if(!fp){
		Zenithra_LogErr(__FILE__, __LINE__, "OBJ file could not be opened");
		Zenithra_LogMsg(fileName);
		return NULL;
	}
	while(1){
		res = fscanf(fp, "%s", buffer);
		if(res == EOF){
			break;
		}

		if(strcmp(buffer, "v") == 0){
			fscanf(fp, "%f %f %f", &node_vert->data[0], &node_vert->data[1], &node_vert->data[2]);
			node_vert->next = Zenithra_CreateNode((void*)&node_vert, false, sizeof(struct list_temp_vec3));
			node_vert = node_vert->next;
		}

		if(strcmp(buffer, "vn") == 0){
			fscanf(fp, "%f %f %f", &node_norm->data[0], &node_norm->data[1], &node_norm->data[2]);
			node_norm->next = Zenithra_CreateNode((void*)&node_norm, false, sizeof(struct list_temp_vec3));
			node_norm = node_norm->next;
		}

		if(strcmp(buffer, "vt") == 0){
			fscanf(fp, "%f %f", &node_text->data[0], &node_text->data[1]);
			node_text->next = Zenithra_CreateNode((void*)&node_text, false, sizeof(struct list_temp_vec2));
			node_text = node_text->next;
		}

		if(strcmp(buffer, "f") == 0){
			triangles++;
			fscanf(fp, "%d/%d/%d", &node_face->data[0][0], &node_face->data[0][1], &node_face->data[0][2]);
			fscanf(fp, "%d/%d/%d", &node_face->data[1][0], &node_face->data[1][1], &node_face->data[1][2]);
			fscanf(fp, "%d/%d/%d", &node_face->data[2][0], &node_face->data[2][1], &node_face->data[2][2]);
			node_face->next = Zenithra_CreateNode((void*)&node_face, false, sizeof(struct list_temp_mat3));
			node_face = node_face->next;
		}
	}
	fclose(fp);

	GLfloat *vertex_buffer_data, *uvs_buffer_data, *normals_buffer_data;
	vertex_buffer_data = (GLfloat*)malloc(sizeof(GLfloat*) * triangles * 3 * 3 + 48);
	uvs_buffer_data = (GLfloat*)malloc(sizeof(GLfloat*) * triangles * 3 * 2 + 24);
	normals_buffer_data = (GLfloat*)malloc(sizeof(GLfloat*) * triangles * 3 * 3 + 48);

	int i, j, a = 0, b = 0;
	node_face = head_face;
	while(1){
		for(i = 0; i < 3; i++){
			node_vert = head_vert;
			node_norm = head_norm;
			node_text = head_text;
			for(j = 1; j < node_face->data[i][0]; j++){
				node_vert = node_vert->next;
			}
			for(j = 1; j < node_face->data[i][1]; j++){
				node_text = node_text->next;
			}
			for(j = 1; j < node_face->data[i][2]; j++){
				node_norm = node_norm->next;
			}
			uvs_buffer_data[b] = node_text->data[0];
			uvs_buffer_data[b+1] = node_text->data[1];
			normals_buffer_data[a] = node_norm->data[0];
			normals_buffer_data[a+1] = node_norm->data[1];
			normals_buffer_data[a+2] = node_norm->data[2];
			vertex_buffer_data[a] = node_vert->data[0];
			vertex_buffer_data[a+1] = node_vert->data[1];
			vertex_buffer_data[a+2] = node_vert->data[2];
			a += 3;
			b += 2;
		}

		if(!node_face->next->next){
			break;
		}
		node_face = node_face->next;
	}

	struct object_data *obj = (void*)malloc(sizeof(struct object_data));

	obj->objSize = triangles * 3;

	glGenBuffers(1, &obj->objVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->objVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data) * triangles * 3 * 3 + 48, &vertex_buffer_data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->objUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->objUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs_buffer_data) * triangles * 3 * 2 + 24, &uvs_buffer_data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->objNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->objNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals_buffer_data) * triangles * 3 * 3 + 48, &normals_buffer_data[0], GL_STATIC_DRAW);

	Zenithra_Free((void*)&vertex_buffer_data);
	Zenithra_Free((void*)&uvs_buffer_data);
	Zenithra_Free((void*)&normals_buffer_data);

	Zenithra_FreeList((void*)&head_vert);
	Zenithra_FreeList((void*)&head_norm);
	Zenithra_FreeList((void*)&head_text);
	Zenithra_FreeList((void*)&head_face);

	engineDataStr->objNum++;
	return obj;
}

GLuint Zenithra_CreateTexture(const char* fileName){
	char fileCheck[4];
	unsigned char* data;
	unsigned char header[124];
	FILE *fp = NULL;

	fp = fopen(fileName, "rb");
	if(!fp){
		Zenithra_LogErr(__FILE__, __LINE__, "BMP file not found");
		Zenithra_LogMsg(fileName);
		return 0;
	}

	fread(fileCheck, 1, 4, fp);
	if(strncmp(fileCheck, "DDS ", 4) != 0){
		Zenithra_LogErr(__FILE__, __LINE__, "Invalid texture file");
		Zenithra_LogMsg(fileName);
		fclose(fp);
		return 0;
	}

	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int bufsize;

	fseek(fp, 0, SEEK_END);
	bufsize = ftell(fp) - 128;
	fseek(fp, 128, SEEK_SET);
	data = (unsigned char*)malloc(bufsize);
	int bytesRead = fread(data, 1, bufsize, fp);
	if(bytesRead != bufsize){
		Zenithra_LogErr(__FILE__, __LINE__, "Failed to read DDS texture data");
		Zenithra_Free((void*)&data);
		fclose(fp);
		return 0;
	}
	fclose(fp);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	unsigned int format;

	char fourCC[4];
	memcpy(fourCC, &header[80], 4);

	if (strncmp(fourCC, "DXT1", 4) == 0)
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	else if (strncmp(fourCC, "DXT3", 4) == 0)
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	else if (strncmp(fourCC, "DXT5", 4) == 0)
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	else {
		Zenithra_LogErr(__FILE__, __LINE__, "Unsupported DDS format");
		Zenithra_Free((void*)&data);
		return 0;
	}

	if (strncmp(fourCC, "DX10", 4) == 0) {
		Zenithra_LogErr(__FILE__, __LINE__, "DX10 DDS files not supported");
		Zenithra_Free((void*)&data);
		return 0;
	}

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;
	bool useMipmaps = mipMapCount > 1;

	unsigned int levels = useMipmaps ? mipMapCount : 1;
	for (unsigned int level = 0; level < levels; ++level) {
		unsigned int w = width > 0 ? width : 1;
		unsigned int h = height > 0 ? height : 1;
		unsigned int size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

		if (offset + size > bufsize) {
			Zenithra_LogErr(__FILE__, __LINE__, "DDS mipmap data overflows buffer");
			Zenithra_Free((void*)&data);
			return 0;
		}

		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, w, h, 0, size, data + offset);
		offset += size;

		if (useMipmaps) {
			width = width > 1 ? width / 2 : 1;
			height = height > 1 ? height / 2 : 1;
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return textureID;
}

void Zenithra_RenderObject(struct in_engine_data *engineDataStr, struct object_data **obj, mat4 *modelMatrices, int objNum, GLuint texID){
	int i;
	mat4 projection, view, mvp;
	glm_perspective(glm_rad(engineDataStr->MOVE->FOV), (float) engineDataStr->window_x / (float) engineDataStr->window_y, 1.0f, 1000.0f, projection);

	vec3 positionDirection;
	glm_vec3_add(engineDataStr->MOVE->directionWalk, engineDataStr->MOVE->position, positionDirection);
	vec3 up = {0.0f, 1.0f, 0.0f};
	glm_lookat(engineDataStr->MOVE->position, positionDirection, up, view);

	glm_mat4_mul(projection, view, mvp);
	glm_mat4_mul(mvp, modelMatrices[objNum], mvp);

    //GLint lightPosLoc = glGetUniformLocation(engineDataStr->GL->programID, "LightPosition_worldspace");
    //glUniform3f(lightPosLoc, 0.0f, 5.0f, 0.0f);

	glUniformMatrix4fv(engineDataStr->GL->matrixID, 1, GL_FALSE, &mvp[0][0]);

	glBindTexture(GL_TEXTURE_2D, texID);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, obj[objNum]->objVertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, obj[objNum]->objUVBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, obj[objNum]->objNormalBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, obj[objNum]->objSize);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, 0);
}