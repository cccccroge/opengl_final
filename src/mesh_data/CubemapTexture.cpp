#include "CubemapTexture.h"
#include "STB/stb_image.h"
#include <iostream>


CubemapTexture::CubemapTexture(const std::vector<std::string> paths)
{
    // record paths
    this->paths = paths;

    glGenTextures(1, &tbo);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tbo);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // loading 6 textures
    if (paths.size() != 6) {
        std::cout << "you're not giving six pics" << std::endl;
    }
    else {
        stbi_set_flip_vertically_on_load(0);

        for (int i = 0; i < paths.size(); ++i) {
            unsigned char *data;
            int width;
            int height;
            int channel;

            data = stbi_load(paths[i].c_str(), &width, &height, &channel, 0);
            if (data != NULL) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, 
                    width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else {
                std::cout << "failed to load one of six pics" << std::endl;
                stbi_image_free(data);
            }
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubemapTexture::~CubemapTexture()
{
    glDeleteTextures(1, &tbo);
}

void CubemapTexture::bind(ShaderProgram &program, const std::string sampler_name, 
        const int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tbo);
	program.bind();
	program.setUniform1i(sampler_name.c_str(), (GLint)index);
}

void CubemapTexture::unbind(const int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
