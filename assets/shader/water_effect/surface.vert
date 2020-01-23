#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;


uniform mat4 mvMatrix;	// model's mv
out FogData
{
	vec4 viewSpace;	// view space position

} fogData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{	
    FragPos = vec3(model * vec4(position, 1.0f));
	gl_Position = projection * view *  model * vec4(position, 1.0f);
    Normal = mat3(transpose(inverse(model))) * normal;  

	fogData.viewSpace = mvMatrix * vec4(position, 1.0);
} 