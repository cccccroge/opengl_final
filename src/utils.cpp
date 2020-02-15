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
#include <stdlib.h>
#include "event/gui.h"



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

bool isFileExist(const char *path)
{
	std::ifstream infile(path);
	return infile.good();
}


unsigned int GlutTimer::count = 0;
bool GlutTimer::enabled = false;
bool GlutTimer::animated = false;
unsigned int GlutTimer::interval = 0;

float randomFloat(const float min, const float max)
{
	return min + (rand() / (RAND_MAX / (max - min)));
}

double calcFPS(double timeInterval)
{
	// Static values which only get initialised the first time the function runs
	static double startTime = glutGet(GLUT_ELAPSED_TIME);
	static double fps = 0.0;           // Set the initial FPS value to 0.0

									   // Set the initial frame count to -1.0 (it gets set to 0.0 on the next line). Because
									   // we don't have a start time we simply cannot get an accurate FPS value on our very
									   // first read if the time interval is zero, so we'll settle for an FPS value of zero instead.
	static double frameCount = -1.0;

	// Here again? Increment the frame count
	frameCount++;

	// Ensure the time interval between FPS checks is sane (low cap = 0.0 i.e. every frame, high cap = 10.0s)
	if (timeInterval < 0.0)
	{
		timeInterval = 0.0;
	}
	else if (timeInterval > 10.0)
	{
		timeInterval = 10.0;
	}

	// Get the duration in seconds since the last FPS reporting interval elapsed
	// as the current time minus the interval start time
	double duration = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000.0;

	// If the time interval has elapsed...
	if (duration > timeInterval)
	{
		// Calculate the FPS as the number of frames divided by the duration in seconds
		fps = frameCount / duration;

		// Convert the fps value into a string using an output stringstream
		std::ostringstream stream;
		stream << fps;
		std::string fpsString = stream.str();

		// Reset the frame count to zero and set the initial time to be now
		frameCount = 0.0;
		startTime = glutGet(GLUT_ELAPSED_TIME);
	}

	// Return the current FPS - doesn't have to be used if you don't want it!
	return fps;
}

std::vector<glm::vec3> genSsaoKernel(const int points)
{
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0~1
	std::default_random_engine generator;

	std::vector<glm::vec3> kernel;
	for (unsigned int i = 0; i < points; ++i)
	{
		// half cube
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,	// -1~1
			randomFloats(generator) * 2.0 - 1.0,	// -1~1
			randomFloats(generator)					//  0~1
		);

		// half sphere surface
		sample = glm::normalize(sample);

		// half sphere
		sample *= randomFloats(generator);

		// make dense when the point is closer to center
		float scale = (float)i / points;
		scale = 0.1f + 0.9f * (scale * scale);
		sample *= scale;

		kernel.push_back(sample);
	}

	return kernel;
}

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

