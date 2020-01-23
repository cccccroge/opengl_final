#version 420 core

layout(location = 0) in vec3 position;

uniform mat4 mMatrixModel;


void main()
{
	// don't need to convert to light space, we will do it in geometry stage
	gl_Position = mMatrixModel * vec4(position, 1.0);
}