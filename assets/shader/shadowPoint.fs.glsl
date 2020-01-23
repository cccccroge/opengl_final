#version 420 core

in vec4 fragPos;

uniform vec3 pointLightPos;
uniform float far_plane;


void main()
{
    // calculate depth
    float lightDistance = length(fragPos.xyz - pointLightPos);
    float depth = lightDistance / far_plane;

    gl_FragDepth = depth;
}