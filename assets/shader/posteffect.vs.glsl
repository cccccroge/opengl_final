#version 420 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoord;

out ScreenData
{
    vec2 texCoord;

} screenData;


void main()
{
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
    screenData.texCoord = texCoord; 
}