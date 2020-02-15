#version 420 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 mMatrix;   // model's m
uniform mat4 vMatrix;  // main camera's v
uniform mat4 pMatrix;  // main camera's p

out SsaoData
{
    vec3 fragPos;   // in view space
    vec3 normal;

} ssaoData;


void main()
{
    ssaoData.fragPos = ((vMatrix * mMatrix) * vec4(position, 1.0)).xyz;
    ssaoData.normal = transpose(inverse(mat3(vMatrix * mMatrix))) * normal;

    gl_Position = (pMatrix * vMatrix * mMatrix) * vec4(position, 1.0);
}