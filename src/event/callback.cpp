#include "callback.h"
#include "../utils.h"
#include "../init.h"
#include "freeGLUT/freeglut.h"

#include "GLM/vec3.hpp"
#include "GLM/ext/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

#include <iostream>
#include <map>
#include <set>
#include "../global.h"
#include "../scene/Model.h"
#include "keymap.h"


// keyboard/mouse status record
std::map<std::string, bool> btnsPressed;
std::set<std::string> pressedBtnSet;

struct _MOUSE_POS
{
	int x;
	int y;

} MOUSE_POS;

// flag helper
bool shifted = false;	// whether mouse movement is shifted by program
TOOL_MODE toolMode = TOOL_MODE::NONE;


void onMenuTriggered(int id)
{
	switch(id)
	{
	case MENU_TIMER_START:
		global::timer.Start();
		break;
	case MENU_TIMER_STOP:
		global::timer.Pause();
		break;
	case MENU_EXIT:
		exitProgram();
		break;
	default:
		break;
	}
}


void onDisplayRefresh(void)
{
	global::renderer->RenderAll();
}


void onWindowReshaped(int width, int height)
{
	// update both CPU and GPU side
	global::renderWidth = width;
	global::renderHeight = height;

	glViewport(0, 0, global::renderWidth, global::renderHeight);
}


void onKeyboardPressed(unsigned char key, int x, int y)
{
	if (toolMode == TOOL_MODE::NONE) {
		std::string keyStr(1, key);
		if (!btnsPressed[keyStr]) {
			pressedBtnSet.insert(keyStr);
			std::cout << "set: ";
			for (auto str : pressedBtnSet) {
				std::cout << str << " ";
			}
			std::cout << std::endl;
			checkTool(keyStr);
			if (toolMode == TOOL_MODE::FIRST_PERSON) {
				// update mouse pos immediately, or it will be uninitialized
				MOUSE_POS.x = x;
				MOUSE_POS.y = y;
			}
			else if (toolMode == TOOL_MODE::TOGGLE_FULLSCREEN) {
				glutFullScreenToggle();
			}
		}

		// model rotation
		switch(key) {
			case 'q':
				global::Man->rotate(3.0f, UP_VECTOR);
				break;
			case 'e':
				global::Man->rotate(-3.0f, UP_VECTOR);
				break;
		}
	}
	else if (toolMode == TOOL_MODE::FIRST_PERSON) {
		switch(key) {
			case 'w':
				global::camViewport.move('f');
				break;

			case 's':
				global::camViewport.move('b');
				break;

			case 'a':
				global::camViewport.move('l');
				break;

			case 'd':
				global::camViewport.move('r');
				break;

			case 'z':
				global::camViewport.move('u');
				break;

			case 'x':
				global::camViewport.move('d');
				break;
				
			case 27:	// ESC
				checkTool("");
				break;

			default:
				break;
		}
	}
}


void onKeyboardReleased(unsigned char key, int x, int y)
{
	std::string keyStr(1, key);
	pressedBtnSet.erase(keyStr);
	std::cout << "set: ";
	for (auto str : pressedBtnSet) {
		std::cout << str << " ";
	}
	std::cout << std::endl;

	if (toolMode == TOOL_MODE::TOGGLE_FULLSCREEN && keyStr == "f") {
		checkTool("");
	}
}


void onSpecialkeysPressed(int key, int x, int y)
{
	if (toolMode == TOOL_MODE::NONE) {
		std::string keyStr = btn2str(key);
		if (!btnsPressed[keyStr]) {
			pressedBtnSet.insert(keyStr);
			std::cout << "set: ";
			for (auto str : pressedBtnSet) {
				std::cout << str << " ";
			}
			std::cout << std::endl;
			checkTool(keyStr);
		}
	}
}


void onSpecialkeysReleased(int key, int x, int y)
{
	std::string keyStr = btn2str(key);
	pressedBtnSet.erase(keyStr);
	std::cout << "set: ";
	for (auto str : pressedBtnSet) {
		std::cout << str << " ";
	}
	std::cout << std::endl;
}


void onMousePressed(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		if (toolMode == TOOL_MODE::NONE) {
			std::string keyStr = btn2str(button);
			if (!btnsPressed[keyStr]) {
				pressedBtnSet.insert(keyStr);
				std::cout << "set: ";
				for (auto str : pressedBtnSet) {
					std::cout << str << " ";
				}
				std::cout << std::endl;

				checkTool(keyStr);
				if (toolMode == TOOL_MODE::ORBIT || 	
					toolMode == TOOL_MODE::PAN) {
					// update mouse pos immediately, or it will be uninitialized
					MOUSE_POS.x = x;
					MOUSE_POS.y = y;
				}
			}
		}

	}

	else if (state == GLUT_UP) {
		std::string keyStr = btn2str(button);
		pressedBtnSet.erase(keyStr);
		std::cout << "set: ";
		for (auto str : pressedBtnSet) {
			std::cout << str << " ";
		}
		std::cout << std::endl;

		if ((toolMode == TOOL_MODE::ORBIT || toolMode == TOOL_MODE::PAN) &&
			button == GLUT_MIDDLE_BUTTON) {
			checkTool("");			
		}
	}	
}


void onMouseMoved(int x, int y)
{
	if (toolMode == TOOL_MODE::ORBIT || 
		toolMode == TOOL_MODE::PAN ||
		toolMode == TOOL_MODE::FIRST_PERSON) {

		onMouseMovedDelta(x - MOUSE_POS.x, y - MOUSE_POS.y);
		MOUSE_POS.x = x;
		MOUSE_POS.y = y;
	}
}


void onMouseMovedDelta(int dx, int dy)
{
	if (toolMode == TOOL_MODE::PAN) {
		global::camViewport.pan(dx, dy);
	}
	else if (toolMode == TOOL_MODE::ORBIT) {
		global::camViewport.orbit(ORBIT_DISTANCE, dx, dy);
	}
	else if (toolMode == TOOL_MODE::FIRST_PERSON) {
		global::camViewport.lookAround(dx, dy);
	}
}


void onWindowClosed(void)
{
	exitProgram();
}


void onMouseWheelSpinned(int wheel, int direction, int x, int y)
{
	if (wheel != 0) {
		std::cout << "spinning non-default wheel!" << std::endl;
		return;
	}

	if (toolMode == TOOL_MODE::NONE) {
		global::camViewport.zoom(direction);
	}
}


bool checkTool(std::string last)
{
	// deactivate
	if (last == "") {
		toolMode = TOOL_MODE::NONE;
		return false;
	}

	// activate if find
	std::set<std::string> exceptLast = pressedBtnSet;
	exceptLast.erase(last);
	std::pair<std::set<std::string>, std::string> currentPair = { exceptLast, last };

	if (keyMap.find(currentPair) != keyMap.end()) {
		toolMode = keyMap[currentPair];
		return true;
	}
	
	// nothing match
	return false;
}