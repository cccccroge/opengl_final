#include "global.h"

/* global variables definition (temporarily) */
GlutTimer global::timer = GlutTimer(16);
Camera global::camViewport = Camera();

float global::renderWidth = 0;
float global::renderHeight = 0;
float global::comp_bar_xCoord = 0;
double global::current_fps = 0;