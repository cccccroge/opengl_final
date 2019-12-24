#ifndef GLOBAL_H
#define GLOBAL_H

#include "utils.h"
#include "scene/Model.h"
#include "scene/Camera.h"
#include "render/ShaderProgram.h"
#include "render/Renderer.h"
#include "render/PostEffectBuffer.h"
#include "scene/Skybox.h"

#endif

/* declaration of global variables */

namespace global
{
    extern GlutTimer timer;
    extern ShaderProgram *program_model;
    extern ShaderProgram *program_posteffect;
    extern ShaderProgram *program_skybox;
    extern ShaderProgram *program_shadow;

    extern PostEffectBuffer *postEffectBuffer;
    extern FrameBuffer *depthMapBuffer;
    extern Texture* depthTex;

    extern Model *Man;
    extern Skybox *skybox;

    extern Camera camViewport;
    extern Camera camLight;
    extern Renderer *renderer;

    extern float renderWidth;
    extern float renderHeight;

    extern float comp_bar_xCoord;
}

