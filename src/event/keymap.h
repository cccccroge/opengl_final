#pragma once

#include "freeGLUT/freeglut.h"
#include <string>
#include <map>
#include <set>


enum class TOOL_MODE {
	NONE,
	ZOOM,
	PAN,
	ORBIT,
	FIRST_PERSON,
	TOGGLE_FULLSCREEN
};

/* map [Pair] to [TOOL_MODE] , each pair has a pressed button set
   and an activate button */
extern std::map< std::pair<std::set<std::string>, std::string>, TOOL_MODE > keyMap;

std::string btn2str(int GLUT_BUTTON);