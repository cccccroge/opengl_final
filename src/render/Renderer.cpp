#include "Renderer.h"
#include "freeGLUT/freeglut.h"
#include "../utils.h"
#include "../global.h"
#include <iostream>

Renderer::Renderer() : main_camera(NULL)
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
    //glEnable(GL_CULL_FACE);
}


Renderer::~Renderer()
{
    
}


void Renderer::setMainCamera(Camera &cam)
{
    main_camera = &cam;
}


void Renderer::setLightCamera(Camera &cam)
{
    light_camera = &cam;
}


void Renderer::addModel(Model &model)
{
    model_vec.push_back(&model);
}

void Renderer::addSkybox(Skybox &_skybox)
{
    skybox = &_skybox;
}

void Renderer::RenderAll()
{
    // 1.draw depth map
    global::depthMapBuffer->bind();
 
    glViewport(0, 0, 1024, 1024);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    global::program_shadow->bind();

    for (auto modelPtr : model_vec) {
        // change uniforms in program
        glm::mat4 model = modelPtr->getModelMat();
        glm::mat4 view = light_camera->getViewMat();
        glm::mat4 proj = light_camera->getProjMat();
        global::program_shadow->setUniformMat4("vpMatrixLight", proj * view);
        global::program_shadow->setUniformMat4("mMatrixModel", model);

        // bind mesh and draw
        for (auto meshPtr : modelPtr->getMeshes()) {
            meshPtr->bind();
	        glDrawElements(GL_TRIANGLES, meshPtr->getIndicesNum(),
                GL_UNSIGNED_INT, 0);
        }
	}

    // 2-1.draw all models
    global::postEffectBuffer->bindFrameBuffer();
    glViewport(0, 0, global::renderWidth, global::renderHeight);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    global::program_model->bind();

    for (auto modelPtr : model_vec) {
        // change uniforms in program
        glm::mat4 model = modelPtr->getModelMat();
        glm::mat4 view = main_camera->getViewMat();
        glm::mat4 proj = main_camera->getProjMat();
        glm::vec3 cameraPos = main_camera->getPos();
        glm::mat4 view_l = light_camera->getViewMat();
        glm::mat4 proj_l = light_camera->getProjMat();

        global::program_model->setUniformMat4("mvpMatrix", proj * view * model);
        global::program_model->setUniformMat4("mMatrix", model);
        global::program_model->setUniformVec3("viewPos", cameraPos);
        global::program_model->setUniformMat4("vpMatrixLight", proj_l * view_l);

        // bind mesh and draw
        for (auto meshPtr : modelPtr->getMeshes()) {
            skybox->getTexture().bind(*global::program_model,   // use in enviroment mapping
                "skybox", 0);
            global::depthTex->bind(*global::program_model,    // use in shadow mapping
                "shadowMap", 1);
            meshPtr->bind(*global::program_model, "tex");
	        glDrawElements(GL_TRIANGLES, meshPtr->getIndicesNum(),
                GL_UNSIGNED_INT, 0);
        }
	}

    // 2-2.draw sky box: draw after all models are shown which is optimized
    glDepthMask(GL_FALSE);

    global::program_skybox->bind();
    glm::mat4 model = skybox->getModelMat();
    glm::mat4 view = glm::mat4( // drop transformation for skybox so that it won't move
        glm::mat3(main_camera->getViewMat()));
    glm::mat4 proj = main_camera->getProjMat();
    global::program_skybox->setUniformMat4("um4mvp", proj * view * model);
    skybox->bind(*global::program_skybox, "skybox", 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthMask(GL_TRUE);

    // 3.draw (step 1~2)'s texture on to 2D space and do post effect if needed
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    global::program_posteffect->bind();
    global::depthTex->bind(*global::program_posteffect, "screenTex", 0);
    //global::postEffectBuffer->bindMeshOnly();
    global::postEffectBuffer->bindScreen();
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 4.Finish all draw calls, now flip swap buffer
    glutSwapBuffers();
}
