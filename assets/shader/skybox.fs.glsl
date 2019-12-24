#version 420 core

uniform samplerCube skybox;
in vec3 texCoord;  // note this should be vec3 as direction vector

out vec4 fragColor;


void main()
{
    fragColor = texture(skybox, texCoord);
}