#version 420 core

in ScreenData
{
    vec2 texCoord;

} screenData;

uniform sampler2D screenTex;


layout(location = 0) out vec4 fragColor;


void main()
{
    fragColor = texture(screenTex, screenData.texCoord);
    // float depthValue = texture(screenTex, screenData.texCoord).r;
    // fragColor = vec4(vec3(depthValue), 1.0);
}