#include"zenithra_core.h"
#include"zenithra_debug.h"

void Zenithra_TestEditor(struct in_engine_data *engineDataStr);

int* Zenithra_ObjectRayIntersectsDetection(float origin[3], struct object_data **obj, struct in_engine_data *engineDataStr) {
    int i, j, k, l = 0, *n;
    float dir[3], edge1[3], edge2[3], h[3], s[3], q[3];
    float length, temp, a, f, u, v, t, *v0, *v1, *v2, distance;
    n = malloc(sizeof(int) * 2);
    n[0] = 0;
    n[1] = 0;

    length = sqrtf(engineDataStr->MOVE->directionLook[0]*engineDataStr->MOVE->directionLook[0] + engineDataStr->MOVE->directionLook[1]*engineDataStr->MOVE->directionLook[1] + 1.0f);
    dir[0] = (engineDataStr->MOVE->directionLook[0] /= length);
    dir[1] = (engineDataStr->MOVE->directionLook[1] /= length);
    dir[2] = (engineDataStr->MOVE->directionLook[2] /= length);

    v0 = &obj[0]->vertex_buffer_data[0];
    v1 = &obj[0]->vertex_buffer_data[3];
    v2 = &obj[0]->vertex_buffer_data[6];

    for(k = 0; k < 3; k++){
        edge1[k] = v1[k] - v0[k];
    }
    for(k = 0; k < 3; k++){
        edge2[k] = v2[k] - v0[k];
    }
    h[0] = dir[1]*edge2[2] - dir[2]*edge2[1];
    h[1] = dir[2]*edge2[0] - dir[0]*edge2[2];
    h[2] = dir[0]*edge2[1] - dir[1]*edge2[0];

    a = edge1[0]*h[0] + edge1[1]*h[1] + edge1[2]*h[2];

    f = 1.0 / a;
    for(k = 0; k < 3; k++){
        s[k] = origin[k] - v0[k];
    }

    u = f * (s[0]*h[0] + s[1]*h[1] + s[2]*h[2]);

    q[0] = s[1]*edge1[2] - s[2]*edge1[1];
    q[1] = s[2]*edge1[0] - s[0]*edge1[2];
    q[2] = s[0]*edge1[1] - s[1]*edge1[0];

    v = f * (dir[0]*q[0] + dir[1]*q[1] + dir[2]*q[2]);

    t = f * (edge2[0]*q[0] + edge2[1]*q[1] + edge2[2]*q[2]);

    distance = t * sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
    temp = distance;

    for(j = 0; j < engineDataStr->objNum; j++){
        for(i = 0; i < obj[j]->objSize-3; i=i+3){
            v0 = &obj[j]->vertex_buffer_data[i + 0];
            v1 = &obj[j]->vertex_buffer_data[i + 3];
            v2 = &obj[j]->vertex_buffer_data[i + 6];

            for(k = 0; k < 3; k++){
                edge1[k] = v1[k] - v0[k];
            }
            for(k = 0; k < 3; k++){
                edge2[k] = v2[k] - v0[k];
            }
            h[0] = dir[1]*edge2[2] - dir[2]*edge2[1];
            h[1] = dir[2]*edge2[0] - dir[0]*edge2[2];
            h[2] = dir[0]*edge2[1] - dir[1]*edge2[0];

            a = edge1[0]*h[0] + edge1[1]*h[1] + edge1[2]*h[2];
            if(fabs(a) < 0.0000001){
                continue;
            }

            f = 1.0 / a;
            for(k = 0; k < 3; k++){
                s[k] = origin[k] - v0[k];
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
            distance = t * sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
            if(t > 0.0000001){
                n[0] = 1;
                if(distance < temp){
                    n[1] = j;
                    temp = distance;
                }
            }
        }
    }
    return n;
}

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
    obj[3] = Zenithra_LoadOBJ(engineDataStr, "./GameData/Objects/thing.obj");
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

        objectRay = Zenithra_ObjectRayIntersectsDetection(engineDataStr->MOVE->position, obj, engineDataStr);
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