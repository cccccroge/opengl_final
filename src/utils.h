#pragma once

#include "GLEW/glew.h"
#include "freeGLUT/freeglut.h"
#include <string>


void dumpInfo(void);

void shaderLog(GLuint shader);

void printGLError();

std::string file2String(const std::string &path);
char* file2Cstring(const char *path);
bool isFileExist(const char *path);
float randomFloat(const float min, const float max);


inline void exitProgram(void)
{
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutLeaveMainLoop();
}

class GlutTimer {

public:
	GlutTimer();
	GlutTimer(unsigned int interval);
	void Start();
	void Pause();
	static unsigned int CurrentSec();
	static void ResetCount();
	static bool enabled;
	static bool animated;

private:
	static void expired(int value);
	static unsigned int count;
	static unsigned int interval;	// msecs
};


