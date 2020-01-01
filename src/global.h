#ifndef GLOBAL_H
#define GLOBAL_H

#include "utils.h"
#include "scene/Model.h"
#include "scene/Camera.h"
#include "render/ShaderProgram.h"
#include "render/Renderer.h"
#include "render/PostEffectBuffer.h"
#include "scene/Skybox.h"
#include "event/Timer.h"
#include "scene/DirectionalLight.h"
#include "scene/PointLight.h"
#include "scene/SpotLight.h"

#endif

/* declaration of global variables */

namespace global
{
    extern GlutTimer timer;
    extern Timer *travelTimer;
    extern ShaderProgram *program_model;
    extern ShaderProgram *program_posteffect;
    extern ShaderProgram *program_skybox;
    extern ShaderProgram *program_shadow;
    extern ShaderProgram *program_shadow_point;

    extern PostEffectBuffer *postEffectBuffer;
    extern FrameBuffer *depthMapBuffer;
    extern Texture* depthTex;
    extern FrameBuffer *depthMapBufferPoint;
    extern CubemapTexture *depthTexPoint;
    extern FrameBuffer* depthMapBufferSpot;
    extern Texture* depthTexSpot;

    extern Model *Man;
    extern Skybox *skybox;
    extern DirectionalLight *sun;
    extern PointLight *pointLight;
    extern SpotLight *spotLight;

    extern Camera camViewport;
    extern Renderer *renderer;

    extern float renderWidth;
    extern float renderHeight;

    extern float comp_bar_xCoord;
}

