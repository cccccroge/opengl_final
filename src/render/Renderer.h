#pragma once

#include "GLEW/glew.h"
#include "../scene/Model.h"
#include "../scene/Camera.h"
#include "../scene/Skybox.h"
#include "../scene/DirectionalLight.h"
#include <vector>


class Renderer
{
public:
    Renderer();
    ~Renderer();
    void setMainCamera(Camera &cam);
    void setLightCamera(Camera &cam);
    void addModel(Model &model);
    void addDirectionalLight(DirectionalLight& light);
    void addSkybox(Skybox &skybox);

    void RenderAll();
	void DrawDepthMap();
	void DrawModels();
	void DrawSkybox();
	void DrawScreen();
	void DrawImGui();

    inline Camera* getMainCamera() { return main_camera; }

private:
    Camera *main_camera;
    Camera *light_camera;
    std::vector<Model *> model_vec;
    std::vector<DirectionalLight*> light_vec;
    Skybox *skybox;
};

