#pragma once

#include "GLEW/glew.h"
#include "../scene/Model.h"
#include "../scene/Camera.h"
#include "../scene/Skybox.h"
#include <vector>


class Renderer
{
public:
    Renderer();
    ~Renderer();
    void setMainCamera(Camera &cam);
    void setLightCamera(Camera &cam);
    void addModel(Model &model);
    void addSkybox(Skybox &skybox);
    void RenderAll();

    inline Camera* getMainCamera() { return main_camera; }

private:
    Camera *main_camera;
    Camera *light_camera;
    std::vector<Model *> model_vec;
    Skybox *skybox;
};

