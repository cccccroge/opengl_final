#version 420 core

layout(location = 0) in vec3 position;

uniform mat4 vpMatrixLight;
uniform mat4 mMatrixModel;


void main()
{
    gl_Position = vpMatrixLight * mMatrixModel * vec4(position, 1.0);
}