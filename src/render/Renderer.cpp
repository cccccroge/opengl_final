#include "Renderer.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glut.h"
#include "imGui/imgui_impl_opengl3.h"
#include "freeGLUT/freeglut.h"
#include "../utils.h"
#include "../global.h"
#include "../event/gui.h"
#include <iostream>
#include "../snow_effect/SnowEffectObject.h"


extern bool snow_effect_enable;
extern bool ocean_effect_enable;

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

	global::depthMapBufferPoint->bind();
	glViewport(0, 0, DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glEnable(GL_CULL_FACE);	// [start cull front face]

		DrawDepthMapPoint();

	global::depthMapBufferSpot->bind();
	glViewport(0, 0, DEPTH_MAP_RESOLUTION, DEPTH_MAP_RESOLUTION);

		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glEnable(GL_CULL_FACE);	// [start cull front face]

		DrawDepthMapSpot();
		
	// 1-2.draw position & normal map for ssao
	global::posNormBuffer->bind();
	glViewport(0, 0, global::renderWidth, global::renderHeight);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawPosNorm();

	// 1-3. draw ssao map
	global::ssaoBuffer->bind();
	glViewport(0, 0, global::renderWidth, global::renderHeight);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		DrawSsaoMap();

		glEnable(GL_DEPTH_TEST);

    // 2.draw all models -> snow -> skybox
    global::postEffectBuffer->bindFrameBuffer();
    glViewport(0, 0, global::renderWidth, global::renderHeight);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		DrawModels();

		if (snow_effect_enable)
			global::snowEffectObj->draw();

		glDisable(GL_CULL_FACE);	// [end cull front face]
		if (ocean_effect_enable)
			global::waterEffectObj->draw();

		glDepthMask(GL_FALSE);
		
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
	glutPostRedisplay();
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

void Renderer::DrawDepthMapPoint()
{
	global::program_shadow_point->bind();

	for (auto modelPtr : model_vec) {
		// change uniforms in program
		glm::mat4 model = modelPtr->getModelMat();
		global::program_shadow_point->setUniformMat4("mMatrixModel", model);

		std::vector<glm::mat4> &lightVPs = global::pointLight->getLightSpaceMat();
		for (int i = 0; i < 6; ++i) {
			std::string name_str = "vpMatrixLight[" + std::to_string(i) + "]";
			global::program_shadow_point->setUniformMat4(name_str.c_str(), lightVPs[i]);
		}

		glm::vec3 pointPos = global::pointLight->getPosition();
		float far_plane = global::pointLight->getFarPlane();
		global::program_shadow_point->setUniformVec3("pointLightPos", pointPos);
		global::program_shadow_point->setUniform1f("far_plane", far_plane);

		// bind mesh and draw
		for (auto meshPtr : modelPtr->getMeshes()) {
			meshPtr->bind();
			glDrawElements(GL_TRIANGLES, meshPtr->getIndicesNum(),
				GL_UNSIGNED_INT, 0);
		}
	}
}

void Renderer::DrawDepthMapSpot()
{
	global::program_shadow->bind();

	for (auto modelPtr : model_vec) {
		// change uniforms in program
		glm::mat4 model = modelPtr->getModelMat();
		glm::mat4 lightVP = global::spotLight->getLightSpaceMat()[0];
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

void Renderer::DrawPosNorm()
{
	global::program_ssao->bind();

	glm::mat4 view = main_camera->getViewMat();
	glm::mat4 proj = main_camera->getProjMat();
	global::program_ssao->setUniformMat4("vMatrix", view);
	global::program_ssao->setUniformMat4("pMatrix", proj);

	for (auto modelPtr : model_vec) {
		// change uniforms in program
		glm::mat4 model = modelPtr->getModelMat();
		global::program_ssao->setUniformMat4("mMatrix", model);

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
		glm::mat4 lightVP_spot = global::spotLight->getLightSpaceMat()[0];
		float far_plane = global::pointLight->getFarPlane();

		global::program_model->setUniformMat4("mvpMatrix", proj * view * model);
		global::program_model->setUniformMat4("mvMatrix", view * model);
		global::program_model->setUniformMat4("mMatrix", model);
		global::program_model->setUniformVec3("viewPos", cameraPos);
		global::program_model->setUniformMat4("vpMatrixLight", lightVP);
		global::program_model->setUniformMat4("vpMatrixLightSpot", lightVP_spot);
		global::program_model->setUniform1f("far_plane", far_plane);

		// bind mesh and draw
		for (auto meshPtr : modelPtr->getMeshes()) {
			global::depthTex->bind(*global::program_model,    // use in shadow mapping
				"shadowMap", 3);		// 0, 1, 2 occupied at line 292
			global::depthTexPoint->bind(*global::program_model,
				"shadowMapPoint", 4);
			global::depthTexSpot->bind(*global::program_model,
				"shadowMapSpot", 5);
			global::ssaoTex->bind(*global::program_model, "ssaoMap", 6);

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

	// testing other buffer texture
	//global::postEffectBuffer->bindMeshOnly();
	//global::depthTex->bind(*global::program_posteffect, "screenTex", 0);
	//global::posTex->bind(*global::program_posteffect, "screenTex", 0);
	//global::normTex->bind(*global::program_posteffect, "screenTex", 0);
	//global::noiseTex->bind(*global::program_posteffect, "screenTex", 0);
	//global::ssaoTex->bind(*global::program_posteffect, "screenTex", 0);

	global::postEffectBuffer->bindScreen();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawSsaoMap()
{
	// setup uniform
	global::program_ssao_map->bind();
	global::posTex->bind(*global::program_ssao_map, "gPosition", 0);
	global::normTex->bind(*global::program_ssao_map, "gNormal", 1);
	global::noiseTex->bind(*global::program_ssao_map, "kernelNoise", 2);
	glm::mat4 proj = main_camera->getProjMat();
	global::program_ssao_map->setUniformMat4("pMatrix", proj);

	// bind vao & draw
	global::postEffectBuffer->bindMeshOnly();
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
