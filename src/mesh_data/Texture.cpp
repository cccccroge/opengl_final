#include "Texture.h"
#include "STB/stb_image.h"
#include <iostream>
#include <random>


Texture::Texture(const char *path, TEXTURE_TYPE t/* = TEXTURE_TYPE::NONE*/) :
    width(0), height(0), data(NULL), mat_type(t), path(path)
{
	stbi_set_flip_vertically_on_load(1);

	int channel;
	data = stbi_load(path, &width, &height, &channel, 4);
	if (data != NULL) {
		// std::cout << "loading pic successfully" << std::endl;
		setUp();
	}
	else {
		std::cout << "failed to load pic" << std::endl;
	}
}

Texture::Texture(const int width, const int height, 
	TEXTURE_TYPE t/* = TEXTURE_TYPE::NONE*/) :
    width(width), height(height), data(NULL), mat_type(t), path(NULL)
{
	setUp();
}

// usage = 0: for depth map texture
//		 = 1: for position/normal map texture
//		 = 2: for noise texture used in ssao
Texture::Texture(int usage, const int width, const int height, 
	TEXTURE_TYPE t/* = TEXTURE_TYPE::NONE*/) :
	width(width), height(height), data(NULL), mat_type(t), path(NULL)
{
	glGenTextures(1, &tbo);
    //glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tbo);

	if (usage == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, 
			GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	else if (usage == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, 
			GL_FLOAT, NULL);
	}
	else if (usage == 2) {
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator;

		std::vector<glm::vec3> noises;
		for (unsigned int i = 0; i < width * height; i++)
		{
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
			noises.push_back(noise);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, 
			GL_FLOAT, &noises[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

    glBindTexture(GL_TEXTURE_2D, 0);
}


Texture::~Texture()
{
	glDeleteTextures(1, &tbo);

	if (data != NULL) {
		stbi_image_free(data);
		data = NULL;
	}
}


/* Model's textures */
void Texture::setUp()
{
    glGenTextures(1, &tbo);
    //glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tbo);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::bind(ShaderProgram &program, const std::string sampler_name, 
	const int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, tbo);
	program.bind();
	program.setUniform1i((sampler_name).c_str(), (GLint)index);
}


void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, tbo);
}