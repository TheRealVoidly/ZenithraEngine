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

    objectRay = malloc(sizeof(int) * 2);
    objectRay[0] = 0;
    objectRay[1] = 0;

    struct object_data **obj;
    obj = (struct object_data**)malloc(4 * sizeof(struct object_data*));
    obj[0] = Zenithra_LoadOBJ(engineDataStr, "./EngineData/VectorArrows/vectorarrowx.obj");
    obj[1] = Zenithra_LoadOBJ(engineDataStr, "./EngineData/VectorArrows/vectorarrowy.obj");
    obj[2] = Zenithra_LoadOBJ(engineDataStr, "./EngineData/VectorArrows/vectorarrowz.obj");
    obj[3] = Zenithra_LoadOBJ(engineDataStr, "./GameData/Objects/tifa.obj");
    GLuint texRed = Zenithra_CreateTexture("./EngineData/Colors/red.DDS");
    GLuint texGreen = Zenithra_CreateTexture("./EngineData/Colors/green.DDS");
    GLuint texBlue = Zenithra_CreateTexture("./EngineData/Colors/blue.DDS");
    //obj[0] = Zenithra_LoadOBJ(engineDataStr, "./Objects/plane.obj");
    //obj[1] = Zenithra_LoadOBJ(engineDataStr, "./Objects/thing.obj");
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

        objectRay = Zenithra_ObjectRayIntersectsDetection(engineDataStr->MOVE->position, obj[3], engineDataStr);
        if(objectRay[0] == 1 && mouseButtonPressed != SDL_BUTTON(3)){
            Zenithra_RenderObject(engineDataStr, obj, 0, texRed);
            Zenithra_RenderObject(engineDataStr, obj, 1, texGreen);
            Zenithra_RenderObject(engineDataStr, obj, 2, texBlue);
            //printf("Cursor is hovering over object num %d\n", objectRay[1]);
            /*for(i = 1; i <= (obj[objectRay[1]]->triangles*3*3+48)-3; i=i+3){
                obj[objectRay[1]]->vertex_buffer_data[i] += 10.0f;
            }
            glBindBuffer(GL_ARRAY_BUFFER, obj[objectRay[1]]->objVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(obj[objectRay[1]]->vertex_buffer_data) * obj[objectRay[1]]->triangles * 3 * 3 + 48, &obj[objectRay[1]]->vertex_buffer_data[0], GL_STATIC_DRAW);*/
        }

        //Zenithra_RenderObject(engineDataStr, obj, 0, texRed);
        //Zenithra_RenderObject(engineDataStr, obj, 1, texGreen);
        //Zenithra_RenderObject(engineDataStr, obj, 2, texBlue);
        Zenithra_RenderObject(engineDataStr, obj, 3, 0);

        SDL_GL_SwapWindow(engineDataStr->SDL->window);
    }while(!programShouldQuit);


    for(i = 0; i < engineDataStr->objNum; i++){
        glDeleteBuffers(1, &obj[i]->objVertexBuffer);
        glDeleteBuffers(1, &obj[i]->objUVBuffer);
        Zenithra_Free((void*)&obj[i]);
    }
}