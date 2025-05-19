#include<GL/glew.h>

const GLchar* vertex_shader_source[] = {
    "#version 330 core\n\
    \
    layout(location = 0) in vec3 vertexPosition;\
    layout(location = 1) in vec2 vertexUV;\
    layout(location = 2) in vec3 vertexNormal;\
    \
    out vec2 UV;\
    out vec3 Position_worldspace;\
    out vec3 Normal_cameraspace;\
    out vec3 EyeDirection_cameraspace;\
    \
    uniform mat4 mvp;\
    uniform mat4 V;\
    uniform mat4 M;\
    \
    void main(){\
        gl_Position = mvp * vec4(vertexPosition, 1.0);\
        Position_worldspace = (M * vec4(vertexPosition, 1.0)).xyz;\
        vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition, 1.0)).xyz;\
        EyeDirection_cameraspace = -vertexPosition_cameraspace;\
        Normal_cameraspace = (V * M * vec4(vertexNormal, 0.0)).xyz;\
        UV = vertexUV;\
    }"
};