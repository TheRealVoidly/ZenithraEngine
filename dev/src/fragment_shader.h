#include<GL/glew.h>

const GLchar* fragment_shader_source[] = {
    "#version 330 core\n\
    \
    in vec2 UV;\
    \
    out vec4 color;\
    \
    uniform sampler2D myTextureSampler;\
    \
    void main(){\
        vec3 MaterialDiffuseColor = texture(myTextureSampler, UV).rgb;\
        vec3 MaterialAmbientColor = 0.1 * MaterialDiffuseColor;\
        color = vec4(MaterialAmbientColor + MaterialDiffuseColor, 1.0);\
    }"
};