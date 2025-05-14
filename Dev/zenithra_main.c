#include"zenithra_core.h"
#include"zenithra_debug.h"

void Zenithra_MainGameLoop(struct in_engine_data *engineDataStr);
void Zenithra_TestEditor(struct in_engine_data *engineDataStr);

bool Zenithra_RayIntersects(float origin[3], float dir[3], struct object_data *obj) {
    int i, x = 0;
    float tmin, tmax, tmp, tymin, tymax, tzmin, tzmax;
    for(i = 0; i <= obj->objSize-5; i=i+3){
        tmin = (obj->vertex_buffer_data[i+0] - origin[0]) / dir[0];
        tmax = (obj->vertex_buffer_data[i+3] - origin[0]) / dir[0];
        if(tmin > tmax){
            tmp = tmin;
            tmin = tmax;
            tmax = tmp;
        }

        tymin = (obj->vertex_buffer_data[i+1] - origin[1]) / dir[1];
        tymax = (obj->vertex_buffer_data[i+4] - origin[1]) / dir[1];
        if(tymin > tymax){
            tmp = tymin;
            tymin = tymax;
            tymax = tmp;
        }

        if(tymin > tmin){
            tmin = tymin;
        }
        if(tymax < tmax){
            tmax = tymax;
        }
        tzmin = (obj->vertex_buffer_data[i+2] - origin[2]) / dir[2];
        tzmax = (obj->vertex_buffer_data[i+5] - origin[2]) / dir[2];
        if(tzmin > tzmax){
            tmp = tzmin;
            tzmin = tzmax;
            tzmax = tmp;
        }

        if((tmin < tzmax) || (tzmin < tmax)){
            if((tmin < tymax) || (tymin < tmax)){
                return true;
            }
        }
    }
    return false;

    /*tmin = (min[0] - origin[0]) / dir[0];
    tmax = (max[0] - origin[0]) / dir[0];
    if(tmin > tmax){
        tmp = tmin;
        tmin = tmax;
        tmax = tmp;
    }

    tymin = (min[1] - origin[1]) / dir[1];
    tymax = (max[1] - origin[1]) / dir[1];
    if(tymin > tymax){
        tmp = tymin;
        tymin = tymax;
        tymax = tmp;
    }

    if((tmin > tymax) || (tymin > tmax)){
        return false;
    }
    if(tymin > tmin){
        tmin = tymin;
    }
    if(tymax < tmax){
        tmax = tymax;
    }
    tzmin = (min[2] - origin[2]) / dir[2];
    tzmax = (max[2] - origin[2]) / dir[2];
    if(tzmin > tzmax){
        tmp = tzmin;
        tzmin = tzmax;
        tzmax = tmp;
    }

    if((tmin > tzmax) || (tzmin > tmax)){
        return false;
    }
    return true;*/
}

int main(int argc, char *argv[]){
    struct in_engine_data *engineDataStr;

    engineDataStr = Zenithra_Init(1200, 700);
    
    //Zenithra_MainGameLoop(engineDataStr);
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

    obj[1]->translationVector[0] = -10.0f; //Move model matrix num 1
    obj[1]->translationVector[1] = 10.0f;
    obj[1]->translationVector[2] = 0.0f;
    glm_translate(modelMatrices[1], obj[1]->translationVector);


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

        if (Zenithra_RayIntersects(engineDataStr->MOVE->position, engineDataStr->MOVE->directionLook, obj[1])) {
            printf("Mouse is hovering over the object!\n");
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

void Zenithra_MainGameLoop(struct in_engine_data *engineDataStr){
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

    obj[1]->translationVector[0] = -10.0f; //Move model matrix num 1
    obj[1]->translationVector[1] = 10.0f;
    obj[1]->translationVector[2] = 0.0f;
    glm_translate(modelMatrices[1], obj[1]->translationVector);

    do{
        lastFrameTime = currentFrameTime;
        currentFrameTime = SDL_GetPerformanceCounter();
        engineDataStr->deltaTime = (double)((currentFrameTime - lastFrameTime) * 1000 / (double)SDL_GetPerformanceFrequency());

        if(!engineDataStr->SDL->focusLost){
            Zenithra_CalcMouseMovement(engineDataStr);
            Zenithra_UpdatePosition(engineDataStr);
        }
        programShouldQuit = Zenithra_HandleEventPoll(engineDataStr);

        glClearColor(0.03f, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(engineDataStr->GL->programID);

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