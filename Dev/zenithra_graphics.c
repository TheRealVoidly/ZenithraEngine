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

struct object_data* Zenithra_LoadOBJ(struct in_engine_data *engineDataStr, bool engineObj, const char* fileName){
	FILE *fp = NULL;
	char buffer[255];
	long long res, i = 0;
	int vi, ti, ni, uv_i;
	int a = 0, b = 0, c = 0;

	struct object_data *obj = (void*)malloc(sizeof(struct object_data));
	obj->triangles = 0;

	GLfloat *uvs_buffer_data, *normals_buffer_data, *temp_vertex_buffer_data, *temp_normals_buffer_data, *temp_uvs_buffer_data;

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

		if(strcmp(buffer, "f") == 0){
			obj->triangles++;
		}
	}

	obj->objSize = obj->triangles * 3;

	int *face_buffer_data;
	face_buffer_data = (int*)malloc(sizeof(int) * (obj->objSize * 3));

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

	obj->vertex_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->objSize * 3));
	uvs_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->objSize * 2));
	normals_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->objSize * 3));
	temp_vertex_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->objSize * 3));
	temp_uvs_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->objSize * 2));
	temp_normals_buffer_data = (GLfloat*)malloc(sizeof(GLfloat) * (obj->objSize * 3));

	fseek(fp, 0, SEEK_SET);
	res = fscanf(fp, "%s", buffer);
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

	for (i = 0; i < obj->objSize * 3; i += 3) {
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

	glGenBuffers(1, &obj->objVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->objVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj->objSize * 3, &obj->vertex_buffer_data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->objUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->objUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj->objSize * 2, &uvs_buffer_data[0], GL_STATIC_DRAW);

	glGenBuffers(1, &obj->objNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->objNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj->objSize * 3, &normals_buffer_data[0], GL_STATIC_DRAW);

	Zenithra_Free((void**)&uvs_buffer_data);
	Zenithra_Free((void**)&normals_buffer_data);
	Zenithra_Free((void**)&face_buffer_data);
	Zenithra_Free((void**)&temp_vertex_buffer_data);
	Zenithra_Free((void**)&temp_normals_buffer_data);
	Zenithra_Free((void**)&temp_uvs_buffer_data);

	obj->engineObj = engineObj;
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
		Zenithra_LogErr(__FILE__, __LINE__, "Unsupported DDS format");
		Zenithra_Free((void*)&data);
		return 0;
	}

	if(strncmp(fourCC, "DX10", 4) == 0){
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

		if(offset + size > bufsize){
			Zenithra_LogErr(__FILE__, __LINE__, "DDS mipmap data overflows buffer");
			Zenithra_Free((void*)&data);
			return 0;
		}

		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, w, h, 0, size, data + offset);
		offset += size;

		if(useMipmaps){
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

void Zenithra_RenderObject(struct in_engine_data *engineDataStr, struct object_data **obj, int objNum, GLuint texID){
	int i;
	mat4 projection, view, mvp;

	if(obj[objNum] == NULL){
		return;
	}

	glm_perspective(glm_rad(engineDataStr->MOVE->FOV), (float) engineDataStr->window_x / (float) engineDataStr->window_y, 1.0f, 1000.0f, projection);

	vec3 positionDirection;
	glm_vec3_add(engineDataStr->MOVE->directionLook, engineDataStr->MOVE->position, positionDirection);
	vec3 up = {0.0f, 1.0f, 0.0f};
	glm_lookat(engineDataStr->MOVE->position, positionDirection, up, view);

	glm_mat4_mul(projection, view, mvp);

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