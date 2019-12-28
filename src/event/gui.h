#pragma once

#include <fstream> 

static bool show_camera_tool = false;
static bool show_weather_tool = false;
static bool show_light_tool = false;
static bool record_mode_enable = false;
static std::ofstream *curve_points_file = NULL;


void setupGui();

void mainMenu();
void cameraTool();
void weatherTool();
void lightTool();