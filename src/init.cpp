#include "GLEW/glew.h"
#include "init.h"
#include "freeGLUT/freeglut.h"
#include "event/callback.h"
#include <iostream>

int initGlutContext(int argc, char *argv[])
{
	glutInitContextVersion(CONTEXT_VER_MAJOR, CONTEXT_VER_MINOR);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	int center_x = (glutGet(GLUT_SCREEN_WIDTH) - MAINWINDOW_WIDTH) / 2;
	int center_y = (glutGet(GLUT_SCREEN_HEIGHT) - MAINWINDOW_HEIGHT) / 2;
	glutInitWindowSize(MAINWINDOW_WIDTH, MAINWINDOW_HEIGHT);
	glutInitWindowPosition(center_x, center_y);

	return glutCreateWindow("My Robot");
}

void initGlew(void)
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << glewGetErrorString(err) << std::endl;
	}
}

void createMenu(void)
{
	int menu_main = glutCreateMenu(onMenuTriggered);
	int menu_timer = glutCreateMenu(onMenuTriggered);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);	

	glutSetMenu(menu_main);
	glutAttachMenu(MENU_TRIGGER_BUTTON);
}

void registerCallbacks(void)
{
	glutDisplayFunc(onDisplayRefresh);
	glutReshapeFunc(onWindowReshaped);
	glutCloseFunc(onWindowClosed);

	glutKeyboardFunc(onKeyboardPressed);
	glutKeyboardUpFunc(onKeyboardReleased);
	glutSpecialFunc(onSpecialkeysPressed);
	glutSpecialUpFunc(onSpecialkeysReleased);
	
	glutMouseFunc(onMousePressed);
	glutMouseWheelFunc(onMouseWheelSpinned);
	glutMotionFunc(onMouseMoved);
	glutPassiveMotionFunc(onMouseMoved);

	//glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);	// set key repeat
}