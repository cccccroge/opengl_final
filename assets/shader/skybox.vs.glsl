#version 420 core

layout(location = 0) in vec3 pos;
uniform mat4 um4mvp;

out vec3 texCoord;  // note this should be vec3 as direction vector


void main()
{
    vec4 position = um4mvp * vec4(pos, 1.0);
    gl_Position = position.xyww; // set z component to 1 after applying perspective division
                            // make it maximum depth value so we can render skybox last (efficient way)
    texCoord = pos;
}