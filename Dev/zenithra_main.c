#include"zenithra_core.h"
#include"zenithra_debug.h"

void Zenithra_TestEditor(struct in_engine_data *engineDataStr);

bool Zenithra_ObjectRayIntersectsDetection(float origin[3], float dir[3], struct object_data *obj) {
    int i, j;
    for(i = 0; i <= obj->objSize-3; i=i+3){
        float edge1[3], edge2[3], h[3], s[3], q[3];
        float a, f, u, v, t;

        float *v0 = &obj->vertex_buffer_data[i + 0];
        float *v1 = &obj->vertex_buffer_data[i + 3];
        float *v2 = &obj->vertex_buffer_data[i + 6];

        for(j = 0; j < 3; ++j){
            edge1[j] = v1[j] - v0[j];
        }
        for(j = 0; j < 3; ++j){
            edge2[j] = v2[j] - v0[j];
        }
        h[0] = dir[1]*edge2[2] - dir[2]*edge2[1];
        h[1] = dir[2]*edge2[0] - dir[0]*edge2[2];
        h[2] = dir[0]*edge2[1] - dir[1]*edge2[0];

        a = edge1[0]*h[0] + edge1[1]*h[1] + edge1[2]*h[2];
        if(fabs(a) < 0.0000001){
            continue;
        }

        f = 1.0 / a;
        for(j = 0; j < 3; ++j){
            s[j] = origin[j] - v0[j];
        }

        u = f * (s[0]*h[0] + s[1]*h[1] + s[2]*h[2]);
        if(u < 0.0 || u > 1.0){
            continue;
        }

        q[0] = s[1]*edge1[2] - s[2]*edge1[1];
        q[1] = s[2]*edge1[0] - s[0]*edge1[2];
        q[2] = s[0]*edge1[1] - s[1]*edge1[0];

        v = f * (dir[0]*q[0] + dir[1]*q[1] + dir[2]*q[2]);
        if(v < 0.0 || u + v > 1.0){
            continue;
        }

        t = f * (edge2[0]*q[0] + edge2[1]*q[1] + edge2[2]*q[2]);
        if(t > 0.0000001){
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[]){
    struct in_engine_data *engineDataStr;

    engineDataStr = Zenithra_Init(1200, 700);

    Zenithra_TestEditor(engineDataStr);

    Zenithra_Destroy(engineDataStr);
    return 0;
}

void Zenithra_TestEditor(struct in_engine_data *engineDataStr){
    int i;
    bool programShouldQuit = false;
    Uint64 lastFrameTime = 0, currentFrameTime = 0;

    struct object_data **obj;
    obj = (struct object_data**)malloc(2 * sizeof(struct object_data*));
    obj[0] = Zenithra_LoadOBJ(engineDataStr, "./Objects/plane.obj");
    obj[1] = Zenithra_LoadOBJ(engineDataStr, "./Objects/thing.obj");
    GLuint texGiga = Zenithra_CreateTexture("./Textures/mugshot1.DDS");
    GLuint texGravel = Zenithra_CreateTexture("./Textures/gravel.DDS");


    mat4 modelMatrices[engineDataStr->objNum];
    for(i = 0; i < engineDataStr->objNum; i++){
        glm_mat4_identity(modelMatrices[i]);
    }

    /*obj[1]->translationVector[0] = -10.0f; //Move model matrix num 1
    obj[1]->translationVector[1] = 10.0f;
    obj[1]->translationVector[2] = 0.0f;
    glm_translate(modelMatrices[1], obj[1]->translationVector);*/


    do{
        lastFrameTime = currentFrameTime;
        currentFrameTime = SDL_GetPerformanceCounter();
        engineDataStr->deltaTime = (double)((currentFrameTime - lastFrameTime) * 1000 / (double)SDL_GetPerformanceFrequency());

        Uint32 mouseButtonPressed = SDL_GetMouseState(NULL, NULL);

        if(!engineDataStr->SDL->focusLost){
            Zenithra_CalcMouseMovement(engineDataStr);

            if(SDL_BUTTON(3) == mouseButtonPressed){
                Zenithra_UpdatePosition(engineDataStr);
            }
        }
        programShouldQuit = Zenithra_HandleEventPoll(engineDataStr);

        glClearColor(0.03f, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(SDL_BUTTON(3) != mouseButtonPressed){
            glUseProgram(0); 
            glPointSize(5.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glEnd();
        }
        glUseProgram(engineDataStr->GL->programID);

        float length = sqrtf(engineDataStr->MOVE->directionLook[0]*engineDataStr->MOVE->directionLook[0] + engineDataStr->MOVE->directionLook[1]*engineDataStr->MOVE->directionLook[1] + 1.0f);
        engineDataStr->MOVE->directionLook[0] /= length;
        engineDataStr->MOVE->directionLook[1] /= length;
        engineDataStr->MOVE->directionLook[2] /= length;

        if (Zenithra_ObjectRayIntersectsDetection(engineDataStr->MOVE->position, engineDataStr->MOVE->directionLook, obj[1]) && SDL_BUTTON(1) == mouseButtonPressed) {
            for(i = 1; i <= obj[1]->triangles*3*3+48; i=i+3){
                obj[1]->vertex_buffer_data[i] += 10.0f;
            }
            glBindBuffer(GL_ARRAY_BUFFER, obj[1]->objVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(obj[1]->vertex_buffer_data) * obj[1]->triangles * 3 * 3 + 48, &obj[1]->vertex_buffer_data[0], GL_STATIC_DRAW);
        }

        Zenithra_RenderObject(engineDataStr, obj, modelMatrices, 0, texGravel);
        Zenithra_RenderObject(engineDataStr, obj, modelMatrices, 1, texGiga);

        SDL_GL_SwapWindow(engineDataStr->SDL->window);
    }while(!programShouldQuit);


    for(i = 0; i < engineDataStr->objNum; i++){
        glDeleteBuffers(1, &obj[i]->objVertexBuffer);
        glDeleteBuffers(1, &obj[i]->objUVBuffer);
        Zenithra_Free((void*)&obj[i]);
    }
}