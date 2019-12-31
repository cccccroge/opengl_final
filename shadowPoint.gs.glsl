#version 420 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

out vec4 fragPos;
uniform mat4 vpMatrixLight[6];


void main()
{
    for (int f = 0; f < 6; ++f)
    {
        gl_Layer = f; // which face
        for (int v = 0; v < 3; ++v) // each triangle's vertices
        {
            fragPos = gl_in[v].gl_Position;
            gl_Position = shadowMatrices[f] * fragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}