#include "Renderer.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glut.h"
#include "imGui/imgui_impl_opengl3.h"
#include "freeGLUT/freeglut.h"
#include "../utils.h"
#include "../global.h"
#include "../event/gui.h"
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

void Renderer::addModel(Model &model)
{
    model_vec.push_back(&model);
}

void Renderer::addDirectionalLight(DirectionalLight& light)
{
	light_vec_dir.push_back(&light);
}

void Renderer::addPointLight(PointLight& light)
{
	light_vec_point.push_back(&light);
}

void Renderer::addSpotLight(SpotLight& light)
{
	light_vec_spot.push_back(&light);
}

void Renderer::addSkybox(Skybox &_skybox)
{
    skybox = &_skybox;
}

void Renderer::RenderAll()
{
    // 1.draw depth map
    global::depthMapBuffer->bind();
    glViewport(0, 0, DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION);

		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);	// [start cull front face]

		DrawDepthMap();

    // 2.draw all models -> skybox
    global::postEffectBuffer->bindFrameBuffer();
    glViewport(0, 0, global::renderWidth, global::renderHeight);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		DrawModels();

		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);	// [end cull front face]
		DrawSkybox();
		glDepthMask(GL_TRUE);

    // 3.draw (step 1~2)'s texture on to 2D space and do post effect if needed
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		DrawScreen();

	// 4.render imGui stuff
	DrawImGui();

    // 5.Finish all draw calls, now flip swap buffer
    glutSwapBuffers();
}

void Renderer::DrawDepthMap()
{
	global::program_shadow->bind();

	for (auto modelPtr : model_vec) {
		// change uniforms in program
		glm::mat4 model = modelPtr->getModelMat();
		glm::mat4 lightVP = global::sun->getLightSpaceMat()[0];
		global::program_shadow->setUniformMat4("vpMatrixLight", lightVP);
		global::program_shadow->setUniformMat4("mMatrixModel", model);

		// bind mesh and draw
		for (auto meshPtr : modelPtr->getMeshes()) {
			meshPtr->bind();
			glDrawElements(GL_TRIANGLES, meshPtr->getIndicesNum(),
				GL_UNSIGNED_INT, 0);
		}
	}
}

void Renderer::DrawModels()
{
	global::program_model->bind();

	global::program_model->setUniform1i("NUM_OF_DIRECTIONAL_LIGHT", light_vec_dir.size());
	global::program_model->setUniform1i("NUM_OF_POINT_LIGHT", light_vec_point.size());
	global::program_model->setUniform1i("NUM_OF_SPOT_LIGHT", light_vec_spot.size());

	for (int i = 0; i < light_vec_dir.size(); ++i) {
		light_vec_dir[i]->bind(*global::program_model, i);
	}
	for (int i = 0; i < light_vec_point.size(); ++i) {
		light_vec_point[i]->bind(*global::program_model, i);
	}
	for (int i = 0; i < light_vec_spot.size(); ++i) {
		light_vec_spot[i]->bind(*global::program_model, i);
	}

	for (auto modelPtr : model_vec) {
		// change uniforms in program
		glm::mat4 model = modelPtr->getModelMat();
		glm::mat4 view = main_camera->getViewMat();
		glm::mat4 proj = main_camera->getProjMat();
		glm::vec3 cameraPos = main_camera->getPos();
		glm::mat4 lightVP = global::sun->getLightSpaceMat()[0];

		global::program_model->setUniformMat4("mvpMatrix", proj * view * model);
		global::program_model->setUniformMat4("mMatrix", model);
		global::program_model->setUniformVec3("viewPos", cameraPos);
		global::program_model->setUniformMat4("vpMatrixLight", lightVP);

		// bind mesh and draw
		for (auto meshPtr : modelPtr->getMeshes()) {
			global::depthTex->bind(*global::program_model,    // use in shadow mapping
				"shadowMap", 1);
			meshPtr->bind(*global::program_model);
			glDrawElements(GL_TRIANGLES, meshPtr->getIndicesNum(),
				GL_UNSIGNED_INT, 0);
		}
	}
}

void Renderer::DrawSkybox()
{
	global::program_skybox->bind();
	glm::mat4 model = skybox->getModelMat();
	glm::mat4 view = glm::mat4( // drop transformation for skybox so that it won't move
		glm::mat3(main_camera->getViewMat()));
	glm::mat4 proj = main_camera->getProjMat();
	global::program_skybox->setUniformMat4("um4mvp", proj * view * model);
	skybox->bind(*global::program_skybox, "skybox", 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::DrawScreen()
{
	global::program_posteffect->bind();

	global::depthTex->bind(*global::program_posteffect, "screenTex", 0);
	//global::postEffectBuffer->bindMeshOnly();
	global::postEffectBuffer->bindScreen();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGLUT_NewFrame();

	setupGui();
	ImGui::Render();

	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
