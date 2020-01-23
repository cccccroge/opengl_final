#include "WaterEffectObject.h"
#include "wave.h"
#include <GLM/gtc/matrix_transform.hpp>
#include "GLM/vec3.hpp"
#include "../global.h"


WaterEffectObject::WaterEffectObject(const glm::vec3 offset, const float speed, const float scale) :
	program_ocean(NULL),
	N(MESH_RESOLUTION), M(MESH_RESOLUTION), indexSize(0), 
	wave_model(NULL), L_x(1000), L_z(1000),
	A(3e-7f), V(10), waterTime(0), modelScale(0.1),
	speed(speed), scale(scale), offset(offset)
{
	omega = glm::vec2(1.0, 1.0);
	lampPos = glm::vec3(0.0f, 50, 0.0);
	sundir = glm::vec3(glm::normalize(glm::vec3(0, 1, -2)));
}

void WaterEffectObject::initBufferObjects()
{
	indexSize = (N - 1) * (M - 1) * 6;
	GLuint *indices = new GLuint[indexSize];

	int p = 0;

	for (int j = 0; j < N - 1; j++)
		for (int i = 0; i < M - 1; i++)
		{
			indices[p++] = i + j * N;
			indices[p++] = (i + 1) + j * N;
			indices[p++] = i + (j + 1) * N;

			indices[p++] = (i + 1) + j * N;
			indices[p++] = (i + 1) + (j + 1) * N;
			indices[p++] = i + (j + 1) * N;
		}

	// Element buffer object
	glGenVertexArrays(1, &surfaceVAO);
	glBindVertexArray(surfaceVAO);
	glGenBuffers(1, &surfaceVBO);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * sizeof(GLuint), indices, GL_STATIC_DRAW);

	delete[] indices;
	wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
	GLfloat vertices[] = {
		// Positions          // Normals           // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// Light
	lampPos = sundir * 50.0f;
	GLuint VBO;
	GLuint lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void WaterEffectObject::initialize()
{
	Shader vertexShader_ocean = Shader(GL_VERTEX_SHADER, 
		"assets/shader/water_effect/surface.vert");
	Shader fragmentShader_ocean = Shader(GL_FRAGMENT_SHADER, 
		"assets/shader/water_effect/surface.frag");
	program_ocean = new ShaderProgram();
	program_ocean->addShader(vertexShader_ocean);
	program_ocean->addShader(fragmentShader_ocean);
	program_ocean->compile();

	initBufferObjects();
}

void WaterEffectObject::draw()
{
	// _WAVE_
	waterTime += speed;
	int nVertex = wave_model->buildTessendorfWaveMesh(waterTime);

	glBindVertexArray(surfaceVAO);
	glBindBuffer(GL_ARRAY_BUFFER, surfaceVBO);

	int size = sizeof(glm::vec3);
	int fieldArraySize = sizeof(glm::vec3) * nVertex;
	glBufferData(GL_ARRAY_BUFFER, fieldArraySize * 2, NULL, GL_STATIC_DRAW);

	// Copy height to buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, fieldArraySize, wave_model->heightField);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Copy normal to buffer
	glBufferSubData(GL_ARRAY_BUFFER, fieldArraySize, fieldArraySize, wave_model->normalField);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)fieldArraySize);
	glEnableVertexAttribArray(1);

	// Clear the colorbuffer
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use cooresponding shader when setting uniforms/drawing objects
	program_ocean->bind();

	lampPos = global::sun->getPosition();
	program_ocean->setUniformVec3("light.position", lampPos);
	program_ocean->setUniformVec3("viewPos", global::camViewport.getPosition());
	program_ocean->setUniform1f("heightMin", wave_model->heightMin * scale * 2);
	program_ocean->setUniform1f("heightMax", wave_model->heightMax * scale * 2);

	// Set lights properties
	/*program_ocean->setUniformVec3("light.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	program_ocean->setUniformVec3("light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));*/
	program_ocean->setUniformVec3("light.specular", glm::vec3(1.0f, 0.9f, 0.7f));

	// Create camera transformations
	glm::mat4 view = global::camViewport.getViewMat();
	glm::mat4 projection = global::camViewport.getProjMat();

	program_ocean->setUniformMat4("view", view);
	program_ocean->setUniformMat4("projection", projection);


	// ===== Draw Model =====
	glBindVertexArray(surfaceVAO);
	glm::mat4 model(1.0);
	
	model = glm::scale(model, glm::vec3(scale));	// Scale the surface 
	model = glm::translate(model, offset);
	program_ocean->setUniformMat4("model", model);

	program_ocean->setUniformMat4("mvMatrix", view * model);
	program_ocean->setUniform1f("fogDensity", 0.03);

	glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//glutSwapBuffers();
	//glutPostRedisplay();
}