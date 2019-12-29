#version 420 core

in ScreenData
{
    vec2 texCoord;

} screenData;

uniform sampler2D screenTex;
uniform float gamma;

layout(location = 0) out vec4 fragColor;


void main()
{
    vec3 color = texture(screenTex, screenData.texCoord).xyz;
    vec3 gamma_correct = pow(color, vec3(1.0 / gamma));
    fragColor = vec4(gamma_correct, 1.0);
}