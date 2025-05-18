#include"zenithra_core.h"
#include"zenithra_debug.h"

void Zenithra_TestEditor(struct in_engine_data *engineDataStr);

int main(int argc, char *argv[]){
    struct in_engine_data *engineDataStr;

    engineDataStr = Zenithra_Init(1200, 700);

    Zenithra_TestEditor(engineDataStr);

    Zenithra_Destroy(engineDataStr);
    return 0;
}

void Zenithra_TestEditor(struct in_engine_data *engineDataStr){
    int i, *objectRay;
    bool programShouldQuit = false;
    Uint64 lastFrameTime = 0, currentFrameTime = 0;

    struct object_data **obj;
    obj = (struct object_data**)malloc(4 * sizeof(struct object_data));
    obj[0] = Zenithra_LoadOBJ(engineDataStr, true, "./EngineData/VectorArrows/vectorarrowx.obj");
    obj[1] = Zenithra_LoadOBJ(engineDataStr, true, "./EngineData/VectorArrows/vectorarrowy.obj");
    obj[2] = Zenithra_LoadOBJ(engineDataStr, true, "./EngineData/VectorArrows/vectorarrowz.obj");
    obj[3] = Zenithra_LoadOBJ(engineDataStr, false, "./GameData/Objects/tifa.obj");
    GLuint texRed = Zenithra_CreateTexture("./EngineData/Colors/red.DDS");
    GLuint texGreen = Zenithra_CreateTexture("./EngineData/Colors/green.DDS");
    GLuint texBlue = Zenithra_CreateTexture("./EngineData/Colors/blue.DDS");
    //GLuint texGiga = Zenithra_CreateTexture("./GameData/Textures/mugshot1.DDS");
    //GLuint texGravel = Zenithra_CreateTexture("./Textures/gravel.DDS");

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
            glPointSize(3.0f);
            glBegin(GL_POINTS);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(0.0f, 0.0f);
            glEnd();
        }
        glUseProgram(engineDataStr->GL->programID);

        objectRay = Zenithra_ObjectRayIntersectsDetection(engineDataStr->MOVE->position, obj, engineDataStr);
        if(objectRay[0] == 1 && mouseButtonPressed != SDL_BUTTON(3)){
            Zenithra_RenderObject(engineDataStr, obj, 0, texRed);
            Zenithra_RenderObject(engineDataStr, obj, 1, texGreen);
            Zenithra_RenderObject(engineDataStr, obj, 2, texBlue);
            for(i = 1; i <= (obj[objectRay[1]]->objSize*3+48)-3; i=i+3){
                obj[objectRay[1]]->vertex_buffer_data[i] += 0.1f;
            }
            glBindBuffer(GL_ARRAY_BUFFER, obj[objectRay[1]]->objVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * obj[objectRay[1]]->objSize * 3, &obj[objectRay[1]]->vertex_buffer_data[0], GL_STATIC_DRAW);
        }
        Zenithra_Free((void**)&objectRay);

        Zenithra_RenderObject(engineDataStr, obj, 3, 0);

        SDL_GL_SwapWindow(engineDataStr->SDL->window);
    }while(!programShouldQuit);


    for(i = 0; i < engineDataStr->objNum; i++){
        glDeleteBuffers(1, &obj[i]->objVertexBuffer);
        glDeleteBuffers(1, &obj[i]->objUVBuffer);
        Zenithra_Free((void*)&obj[i]);
    }
}