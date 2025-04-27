#include"zenithra_core.h"
#include"zenithra_debug.h"

int main(int argc, char *argv[]){
    struct in_engine_data *engineDataStr;

    engineDataStr = Zenithra_Init(1200, 700);
    
    Zenithra_MainGameLoop(engineDataStr);

    Zenithra_Destroy(engineDataStr);
    return 0;
}

void Zenithra_MainGameLoop(struct in_engine_data *engineDataStr){
    int i;
    bool programShouldQuit = false;
    Uint64 lastFrameTime = 0, currentFrameTime = 0;


    struct object_data **obj;
    obj = (struct object_data**)malloc(2 * sizeof(struct object_data*));
    obj[0] = Zenithra_LoadOBJ(engineDataStr, "./Objects/plane.obj");
    //obj[1] = Zenithra_LoadOBJ(engineDataStr, "./Objects/thing.obj");
    GLuint texID = Zenithra_CreateTexture("./Textures/gravel.DDS");


    mat4 modelMatrices[engineDataStr->objNum];
    for(i = 0; i < engineDataStr->objNum; i++){
        glm_mat4_identity(modelMatrices[i]);
    }

    /*obj[1]->translationVector[0] = -10.0f; //Move model matrix num 1
    obj[1]->translationVector[1] = 0.0f;
    obj[1]->translationVector[2] = 0.0f;
    glm_translate(modelMatrices[1], obj[1]->translationVector);*/

    do{
        lastFrameTime = currentFrameTime;
        currentFrameTime = SDL_GetPerformanceCounter();
        engineDataStr->deltaTime = (double)((currentFrameTime - lastFrameTime) * 1000 / (double)SDL_GetPerformanceFrequency());

        if(!engineDataStr->SDL->focusLost){
            Zenithra_CalcMouseMovement(engineDataStr);
            Zenithra_UpdatePosition(engineDataStr);
        }
        programShouldQuit = Zenithra_HandleEventPoll(engineDataStr);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(engineDataStr->GL->programID);

        Zenithra_RenderObject(engineDataStr, obj, modelMatrices, 0, texID);
        //Zenithra_RenderObject(engineDataStr, obj, modelMatrices, 1, 0);

        SDL_GL_SwapWindow(engineDataStr->SDL->window);
    }while(!programShouldQuit);


    for(i = 0; i < engineDataStr->objNum; i++){
        glDeleteBuffers(1, &obj[i]->objVertexBuffer);
        glDeleteBuffers(1, &obj[i]->objUVBuffer);
        Zenithra_Free((void*)&obj[i]);
    }
}

//TEST