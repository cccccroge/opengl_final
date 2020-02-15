#version 420 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;

in SsaoData
{
    vec3 fragPos;   // in view space
    vec3 normal;

} ssaoData;


void main()
{
    gPosition = ssaoData.fragPos;
    gNormal = normalize(ssaoData.normal);
}