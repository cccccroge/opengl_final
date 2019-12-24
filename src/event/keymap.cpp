#include "keymap.h"
#include <iostream>


std::map< std::pair<std::set<std::string>, std::string>, TOOL_MODE > keyMap = {
        { 
            { { "shift_L" }, "mouse_M" }, TOOL_MODE::PAN
        },
        { 
            { std::set<std::string>(), "mouse_M" }, TOOL_MODE::ORBIT
        },
        /*{
            { {?} }, TOOL_MODE::ZOOM  // ignore, GLUT treat mouse wheel as seperated callback
        },*/
        {
            { { "shift_L" }, "F" }, TOOL_MODE::FIRST_PERSON
        },
        {
            { std::set<std::string>(), "f" }, TOOL_MODE::TOGGLE_FULLSCREEN
        },
    };

std::string btn2str(int GLUT_BUTTON)
{
    if (GLUT_BUTTON == GLUT_KEY_SHIFT_L)
        return "shift_L";
    else if (GLUT_BUTTON == GLUT_KEY_SHIFT_R)
        return "shift_R";
    else if (GLUT_BUTTON == GLUT_MIDDLE_BUTTON)
        return "mouse_M";
    else if (GLUT_BUTTON == GLUT_LEFT_BUTTON)
        return "mouse_L";
    else if (GLUT_BUTTON == GLUT_RIGHT_BUTTON)
        return "mouse_R";
    else {
        std::cout << "can't convert GLUT_BUTTON to string" << std::endl;
        return "";
    }
        
}