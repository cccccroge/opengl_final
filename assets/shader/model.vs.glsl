#version 420 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tanSpace;

uniform mat4 mvpMatrix; // model's mvp
uniform mat4 mMatrix;   // model's m
uniform mat4 vpMatrixLight;
uniform mat4 vpMatrixLightSpot;

out VertexData
{
    vec3 position;
    vec3 normal;
    vec2 texCoord;
	mat3 TBN;

} vertexData;

out BlinnPhongData
{
    vec3 fragPos;
    vec3 normal;    // in world space

} blinnPhongData;

out ShadowData
{
    vec4 fragPosLight;   // in light space
    vec4 fragPosLightSpot;

    vec3 fragPosWorld;   // in world space

} shadowData;


void main()
{
    // pass to fragment shader
    vertexData.position = position;
    vertexData.normal = normal;
    vertexData.texCoord = texCoord;

	vec3 T = normalize(vec3(mMatrix * vec4(tanSpace, 0.0)));
	vec3 N = normalize(vec3(mMatrix * vec4(normal, 0.0)));
	vec3 B = cross(N, T);
	vertexData.TBN = mat3(T, B, N);


    blinnPhongData.fragPos = vec3(mMatrix * vec4(position, 1.0));
    blinnPhongData.normal = mat3(transpose(inverse(mMatrix))) * normal;

    shadowData.fragPosLight = vpMatrixLight * vec4(blinnPhongData.fragPos, 1.0);
    shadowData.fragPosLightSpot = vpMatrixLightSpot * vec4(blinnPhongData.fragPos, 1.0);
    shadowData.fragPosWorld = blinnPhongData.fragPos;

    // calculate vertex position
	gl_Position = mvpMatrix * vec4(position, 1.0);
}