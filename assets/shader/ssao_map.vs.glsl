#version 420 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 texCoordVec2;


void main()
{
    texCoordVec2 = texCoord;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}