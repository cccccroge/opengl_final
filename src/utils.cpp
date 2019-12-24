#include "utils.h"
#include <iostream>
#include "freeGLUT/freeglut.h"
#include "GLM/glm.hpp"
#include <string>
#include <sstream>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <cstdio>
//#include <unistd.h>
#include "global.h"



// Print OpenGL context related information.
void dumpInfo(void)
{
	std::cout << "***** OpenGL info *****" << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl; 
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl; 
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl; 
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << std::endl;

	std::cout << "***** GLEW info *****" << std::endl;
	std::cout << "Version: " << GLEW_VERSION_MAJOR << "." << 
		GLEW_VERSION_MINOR << "." << GLEW_VERSION_MICRO << std::endl;
	std::cout << std::endl;

	std::cout << "***** freeGLUT info *****" << std::endl;
	std::stringstream ss;
	ss << glutGet(GLUT_VERSION);
	std::cout << "Version: " << ss.str() << std::endl;
	std::cout << std::endl;

	std::cout << "***** GLM info *****" << std::endl;
	std::cout << "Version: " << GLM_VERSION_MESSAGE << std::endl;
}

void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		std::cout << std::string(errorLog) << std::endl; 
		delete[] errorLog;
	}
}

void printGLError()
{
    GLenum code = glGetError();
    switch(code)
    {
    case GL_NO_ERROR:
        std::cout << "GL_NO_ERROR" << std::endl;
        break;
    case GL_INVALID_ENUM:
        std::cout << "GL_INVALID_ENUM" << std::endl;
        break;
    case GL_INVALID_VALUE:
        std::cout << "GL_INVALID_VALUE" << std::endl;
        break;
    case GL_INVALID_OPERATION:
        std::cout << "GL_INVALID_OPERATION" << std::endl;
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
        break;
    case GL_OUT_OF_MEMORY:
        std::cout << "GL_OUT_OF_MEMORY" << std::endl;
        break;
    case GL_STACK_UNDERFLOW:
        std::cout << "GL_STACK_UNDERFLOW" << std::endl;
        break;
    case GL_STACK_OVERFLOW:
        std::cout << "GL_STACK_OVERFLOW" << std::endl;
        break;
    default:
        std::cout << "GL_ERROR" << std::endl;
    }
}


std::string file2String(const std::string &path)
{
	std::fstream fs;
	fs.open(path);
	if (fs.fail())
		std::cout << "file \"" << path << "\" doesn't exist" << std::endl;
	else
		fs.close();
		
    std::ifstream in(path);
    std::string contents((std::istreambuf_iterator<char>(in)), 
        std::istreambuf_iterator<char>());

    return contents;
}

char* file2Cstring(const char* file)
{
    FILE* fp = fopen(file, "rb");
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *src = new char[sz + 1];
    fread(src, sizeof(char), sz, fp);
    src[sz] = '\0';
    char **srcp = new char*[1];
    srcp[0] = src;
    return *srcp;
}


unsigned int GlutTimer::count = 0;
bool GlutTimer::enabled = false;
bool GlutTimer::animated = false;
unsigned int GlutTimer::interval = 0;


GlutTimer::GlutTimer()
{
	
}

GlutTimer::GlutTimer(unsigned int _interval)
{
 	GlutTimer::enabled = true;
	GlutTimer::animated = false;
	GlutTimer::interval = _interval;
}

void GlutTimer::Start()
{
	glutTimerFunc(interval, this->expired, 0);
	enabled = true;
}

void GlutTimer::Pause()
{
	enabled = false;
}


void GlutTimer::expired(int value)
{
	if (enabled) {
		count += interval;
		glutPostRedisplay();
		glutTimerFunc(interval, expired, 0);	// register one more

		if (GlutTimer::animated) {
			// hard-code animation
			// global::Torso->translate(0.0f, 0.025f, 0.0f);
			// global::Head->rotate(0.5f, std::vector<float>({ 1.0f, 0.0f, 0.0f }));
			// global::Arm_R1->rotate(0.6f, std::vector<float>({ 1.0f, 0.0f, 0.0f }));
			// global::Arm_R2->rotate(0.5f, std::vector<float>({ 1.0f, 0.0f, 0.0f }));
			// global::Arm_L1->rotate(-0.4f, std::vector<float>({ 0.0f, 0.0f, 1.0f }));
			// global::Arm_L2->rotate(0.6f, std::vector<float>({ 0.0f, 0.0f, 1.0f }));
			// global::Leg_R1->rotate(-0.1f, std::vector<float>({ 1.0f, 0.0f, 0.0f }));
			// global::Leg_R1->rotate(0.1f, std::vector<float>({ 0.0f, 0.0f, 1.0f }));
			// global::Leg_R2->rotate(-0.2f, std::vector<float>({ 1.0f, 0.0f, 0.0f }));
			// global::Leg_L1->rotate(-0.1f, std::vector<float>({ 1.0f, 0.0f, 0.0f }));
			// global::Leg_L1->rotate(-0.1f, std::vector<float>({ 0.0f, 0.0f, 1.0f }));
			// global::Leg_L2->rotate(-0.2f, std::vector<float>({ 1.0f, 0.0f, 0.0f }));

			// if (GlutTimer::CurrentSec() >= 3) {
			// 	GlutTimer::ResetCount();
			// 	global::Torso->resetTransformation();
			// 	global::Head->resetTransformation();
			// 	global::Arm_R1->resetTransformation();
			// 	global::Arm_R2->resetTransformation();
			// 	global::Arm_L1->resetTransformation();
			// 	global::Arm_L2->resetTransformation();
			// 	global::Leg_R1->resetTransformation();
			// 	global::Leg_R2->resetTransformation();
			// 	global::Leg_L1->resetTransformation();
			// 	global::Leg_L2->resetTransformation();
			// }
		}
	}
}

unsigned int GlutTimer::CurrentSec()
{
	float sec = (float)count / 1000.0f;
	return round(sec);
}


void GlutTimer::ResetCount()
{
	GlutTimer::count = 0;
}

