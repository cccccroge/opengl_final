#pragma once

#include <string>


void onMenuTriggered(int id);
/* GLUT callback. Called to draw the scene. */
void onDisplayRefresh(void);
/* Setting up viewing matrix */
void onWindowReshaped(int width, int height);
void onWindowClosed(void);

void onKeyboardPressed(unsigned char key, int x, int y);
void onKeyboardReleased(unsigned char key, int x, int y);
void onSpecialkeysPressed(int key, int x, int y);
void onSpecialkeysReleased(int key, int x, int y);

void onMousePressed(int button, int state, int x, int y);
void onMouseMoved(int x, int y);
void onMouseMovedDelta(int dx, int dy);
void onMouseWheelSpinned(int wheel, int direciton, int x, int y);

/* Check whether need to activate any tool or change mode, 
   return true if has activate something, false otherwise */
bool checkTool(std::string last);