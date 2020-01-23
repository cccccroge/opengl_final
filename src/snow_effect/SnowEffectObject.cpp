#include "GLEW/glew.h"
#include "SnowEffectObject.h"
#include "ShaderSnow.h"
#include "../init.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include <vector>
#include <random>
#include "ModelSnow.h"
#include "Accumulation.h"
#include "../global.h"
#include "GroundPlane.h"
#include "Deformation.h"

using namespace std;
using namespace glm;


SnowEffectObject::SnowEffectObject(const glm::vec3 offset, 
	const float gravity, const glm::vec2 windspeed, const float density) :
	offset(offset), gravity(gravity), windspeed(windspeed), density(density)
{
	
}

void SnowEffectObject::initialize()
{
	colourShader = new ShaderSnow(
		"assets/shader/snow_effect/SimpleVertexShader.vertexshader", 
		"assets/shader/snow_effect/SimpleFragmentShader.fragmentshader");
	colourShaderModelID = glGetUniformLocation(colourShader->Program, "model");
	colourShaderViewID = glGetUniformLocation(colourShader->Program, "view");
	colourShaderProjectionID = glGetUniformLocation(colourShader->Program, "projection");
	colourShaderColourID = glGetUniformLocation(colourShader->Program, "userColour");
	glUniform3f(colourShaderColourID, 0.0f, 0.0f, 0.0f);

	heightShader = new ShaderSnow(
		"assets/shader/snow_effect/HeightVertexShader.vertexshader",
		"assets/shader/snow_effect/HeightFragmentShader.fragmentshader");
	heightShaderModelID = glGetUniformLocation(heightShader->Program, "model");
	heightShaderViewID = glGetUniformLocation(heightShader->Program, "view");
	heightShaderProjectionID = glGetUniformLocation(heightShader->Program, "projection");
	GLuint heightShaderColourID = glGetUniformLocation(heightShader->Program, "userColour");
	glUniform3f(heightShaderColourID, 0.0f, 0.0f, 0.0f);

	ShaderSnow textureShader("assets/shader/snow_effect/TexturedVertShader.vertexshader", 
		"assets/shader/snow_effect/TexturedFragShader.fragmentshader");
	GLuint texShaderModelID = glGetUniformLocation(textureShader.Program, "model");
	GLuint texShaderViewID = glGetUniformLocation(textureShader.Program, "view");
	GLuint texShaderProjectionID = glGetUniformLocation(textureShader.Program, "projection");

	instancedShader = new ShaderSnow(
		"assets/shader/snow_effect/InstancedVertShader.vertexshader",
		"assets/shader/snow_effect/InstancedFragShader.fragmentshader");
	Matrices_binding = 0;
	GLuint uniform_block_index = glGetUniformBlockIndex(instancedShader->Program, "Matrices");
	glUniformBlockBinding(instancedShader->Program, uniform_block_index, Matrices_binding);

	// create uniform buffer
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, (NUMSNOWFLAKES + 1) * sizeof(float) * 4 * 4, 0, GL_STREAM_DRAW);


	/////////////////////////////////////////////
	//	SNOWFLAKES
	/////////////////////////////////////////////
	// fill the ModelSnow matrix array

	std::random_device dseeder;
	std::mt19937 rng(dseeder());
	std::uniform_real_distribution<double> genDub(0, 1); //(min, max)

	for (int i = 0; i < NUMSNOWFLAKES; i++)
	{
		GLfloat x = rand() % (kBoundsWidth - 4) + offset[0];
		GLfloat y = rand() % 200 + offset[1];
		GLfloat z = rand() % (kBoundsWidth - 4) + offset[2];
		positions.push_back(vec3(x, y, z));
		ModelMatrices[i] = glm::translate(glm::mat4(1.0f), positions[i]);

		glm::vec3 mass = glm::vec3(0.1f, genDub(rng), 0.1f);
		masses.push_back(mass);
	}

	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 4 * 4, NUMSNOWFLAKES * sizeof(float) * 4 * 4, ModelMatrices);

	/////////////////////////////////////////////
	//	INSTANCING
	/////////////////////////////////////////////
	// vao and vbo handle
	GLuint vbo, ibo;

	snowflake = new ModelSnow("assets/model/snowflake/Snowflake.obj");
	//GLuint vao, vbo, ibo;
	// generate and bind the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// generate and bind the vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// fill with data
	glBufferData(GL_ARRAY_BUFFER, snowflake->meshes[0].vertices.size() * sizeof(Vertex), &snowflake->meshes[0].vertices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
	// Vertex Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
	// Vertex Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));

	// generate and bind the index buffer object
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	// fill with data
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, snowflake->meshes[0].indices.size() * sizeof(GLuint), &snowflake->meshes[0].indices[0], GL_STATIC_DRAW);

	/////////////////////////////////////////////////////////////////////
	// Object  Initialissation
	/////////////////////////////////////////////////////////////////////

	////INSTANCING
	instancedShader->Use();
	ModelSnow snowTest("assets/model/snowflake/Snowflake.obj");	// this can't be commented for some fucking weird reasons

	//GroundPlane* groundPlane = new GroundPlane(colourShader.Program); //for some reason this has to be after the Models are loaded

	/////////////////////////////////////////////////////////////////////
	// Lighting
	/////////////////////////////////////////////////////////////////////
	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

	/////////////////////////////////////////////////////////////////////
	// Tesselation
	/////////////////////////////////////////////////////////////////////
	/*
	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices); //32 on Mellor PCs
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	*/

	//https://en.wikipedia.org/wiki/Delaunay_triangulation
	//https://github.com/Bl4ckb0ne/delaunay-triangulation	
	//http://stackoverflow.com/questions/11634581/tessellate-a-plane-of-points
	 acm = new Accumulation(colourShader->Program);


}

static bool resetAccumulation = false;
static bool isMeltOn = false;
static bool isSnowOn = true;

static int meltTimer = 0;
static int meltTime = 5;

void SnowEffectObject::draw()
{
	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	// Compute the MVP matrix from keyboard and mouse input
	//computeMatricesFromInputs(window);

	/////////////////////////////////////////////////////////////////////////
	//// UPDATE POSITIONS
	/////////////////////////////////////////////////////////////////////////
	//if (resetAccumulation)
	//{
	//	acm->ResetAccumulation();
	//	resetAccumulation = false;
	//}

	////*************************************//
	//// Deformation
	////*************************************//

	//glm::vec3 cubePos(0, 10, 0);
	//glm::vec3 cubeScale(50, 50, 50);
	////acm->RemovePoint(cubePos, cubeScale);
	///*vec3 secondTrack = getControlledCubePostionVec();
	//secondTrack.z -= 20;
	//if (secondMarker)
	//	acm->RemovePoint(secondTrack, controlledCubeScale);*/

	////*************************************//
	//// Melting
	////*************************************//
	//if (isMeltOn)
	//{
	//	meltTimer += t;

	//	if (meltTimer > meltTime)
	//	{
	//		acm->Melt();

	//		meltTimer = 0;
	//	}
	//}

	//*************************************//
	// Snowflakes
	//*************************************//
	if (isSnowOn)
	{
		//https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331
		for (int i = 0; i < NUMSNOWFLAKES; i++)
		{
			//*************************************//
			// Falling Dynamics
			//*************************************//
			glm::vec3 grav = glm::vec3(1.0f, -gravity, 1.0f);
			glm::vec3 wind = glm::vec3(windspeed[0], 1.0, windspeed[1]);
			glm::vec3 force = masses[i] * (grav * wind);
			positions[i].x += force.x;
			positions[i].y += force.y;
			positions[i].z += force.z;

			//*************************************//
			// Accumulation
			//*************************************//
			if (positions[i].y < -10)
			{
				acm->AddPoint(positions[i]);
				positions[i].x = rand() % (int)(kBoundsWidth / density) + offset[0];
				positions[i].y = rand() % 200 + offset[1];
				positions[i].z = rand() % (int)(kBoundsWidth / density) + offset[2];
			}
			ModelMatrices[i] = glm::translate(mat4(1.0f), positions[i]);
			ModelMatrices[i] = glm::scale(ModelMatrices[i], vec3(0.01f, 0.01f, 0.01f));
			ModelMatrices[i] = glm::rotate(ModelMatrices[i], t + i, vec3(0.1f, 0.1f, 0.1f));
		}
		//cout << positions[0].x << "	" << positions[0].y << "	" << positions[0].z << "	" << endl << endl;
		instancedShader->Use();
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 4 * 4, NUMSNOWFLAKES * sizeof(float) * 4 * 4, ModelMatrices);
	}


	///////////////////////////////////////////////////////////////////////
	//// DRAW
	///////////////////////////////////////////////////////////////////////
	//http://roxlu.com/2014/028/opengl-instanced-rendering

	// calculate ViewProjection matrix
	glm::mat4 Projection = global::camViewport.getProjMat();
	// translate the world/view position
	glm::mat4 View = global::camViewport.getViewMat();
	glm::mat4 ViewProjection = Projection * View;
	glm::mat4 model = glm::mat4(1.0);

	//*************************************//
	// Snowflakes
	//*************************************//
	if (isSnowOn)
	{
		// use the shader program
		instancedShader->Use();
		glUniform3f(colourShaderColourID, 0.0f, 0.0f, 0.0f);

		// set the ViewProjection in the uniform buffer
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 4 * 4, glm::value_ptr(ViewProjection));
		glBindBufferRange(GL_UNIFORM_BUFFER, Matrices_binding, ubo, 0, sizeof(float) * 4 * 4 * NUMSNOWFLAKES);
		// bind the vao
		glBindVertexArray(vao);
		// the additional parameter indicates how many instances to render
		glDrawElementsInstanced(GL_TRIANGLES, snowflake->meshes[0].indices.size(), GL_UNSIGNED_INT, 0, NUMSNOWFLAKES);
		glBindVertexArray(0);
	}

	//*************************************//
	// Deformation
	//*************************************//
	// Use our shader
	//colourShader->Use();
	//glm::mat4 ProjectionMatrix = global::camViewport.getProjMat();
	//glm::mat4 ViewMatrix = global::camViewport.getViewMat();
	//glUniformMatrix4fv(colourShaderProjectionID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	//glUniformMatrix4fv(colourShaderViewID, 1, GL_FALSE, &ViewMatrix[0][0]);
	//model = glm::mat4(1.0);
	//model = glm::translate(model, glm::vec3(-100, -10, -50));
	//glUniformMatrix4fv(colourShaderModelID, 1, GL_FALSE, &model[0][0]);

	/*GroundPlane gp(colourShader->Program);
	gp.Draw();*/

	//Deformation* dfm = new Deformation(colourShader->Program);
	//dfm->scale = cubeScale;
	//dfm->Draw(cubePos);	// this will draw a red cube

	/*if (secondMarker)
		dfm->Draw(secondTrack);*/

	//*************************************//
	// Accumulation
	//*************************************//
	//heightShader->Use();
	//glUniformMatrix4fv(heightShaderProjectionID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	//glUniformMatrix4fv(heightShaderViewID, 1, GL_FALSE, &ViewMatrix[0][0]);
	//model = glm::mat4(1.0);
	//model = glm::translate(model, glm::vec3(-100, -10, -50));
	//glUniformMatrix4fv(heightShaderModelID, 1, GL_FALSE, &model[0][0]);
	//acm->Draw();


	//*************************************//
	// Debug Misc
	//*************************************//

	//if (isDebugLines)
	//{
	//	colourShader.Use();
	//	ProjectionMatrix = getProjectionMatrix();
	//	ViewMatrix = getViewMatrix();
	//	glUniformMatrix4fv(colourShaderProjectionID, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	//	glUniformMatrix4fv(colourShaderViewID, 1, GL_FALSE, &ViewMatrix[0][0]);
	//	model = glm::mat4(1.0);
	//	glUniformMatrix4fv(colourShaderModelID, 1, GL_FALSE, &model[0][0]);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	acm->Draw();
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//}

	// Draw tweak bars
	//colourShader.Use();
	//time = glfwGetTime();
	//TwRefreshBar(bar);
	//TwDraw();

}