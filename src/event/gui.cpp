#include "gui.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glut.h"
#include "imGui/imgui_impl_opengl3.h"
#include "../utils.h"
#include "../global.h"
#include <iostream>
#include <fstream> 
#include "GLM/trigonometric.hpp"
#include "../scene/Light.h"
#include "../scene/DirectionalLight.h"

void setupGui()
{
	mainMenu();
	if (show_camera_tool)	cameraTool();
	if (show_weather_tool)	weatherTool();
	if (show_light_tool)	lightTool();
}

void mainMenu()
{
	// main menu at the top
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Application"))
		{
			if (ImGui::MenuItem("Toggle FPS")) {
				//TODO: FPS code
			}

			if (ImGui::MenuItem("Reset to default")) {
				//TODO: Need other settings
			}

			if (ImGui::MenuItem("Exit")) {
				exitProgram();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools")) {

			if (ImGui::MenuItem("Camera tool", NULL, &show_camera_tool));

			if (ImGui::MenuItem("Weather tool", NULL, &show_weather_tool));

			if (ImGui::MenuItem("Light tool", NULL, &show_light_tool));

			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Help")) {

		}

		ImGui::EndMainMenuBar();
	}
}

static float fov = 80.0f;
static float gamma = 1.5f;
static float move_progress = 0.0;

void cameraTool()
{
	ImGui::Begin("Camera tool");

	ImGui::BulletText("Camera setting");
		ImGui::Indent();
		if (ImGui::SliderFloat("fov", &fov, 30.0f, 150.0f)) {
			global::camViewport.setFov(fov);
		}
		if (ImGui::SliderFloat("gamma value", &gamma, 1.0f, 4.0f)) {
			global::program_posteffect->bind();
			global::program_posteffect->setUniform1f("gamma", gamma);
		}
		ImGui::Unindent();
	ImGui::Separator();

	ImGui::BulletText("Static mode");
		ImGui::Indent();
		static int p = 0;
		if (ImGui::RadioButton("place A", &p, 0)) {
			global::camViewport.setPos(glm::vec3(3.44579, 11.9493, 3.00064));
			global::camViewport.setYaw(-114.4f);
			global::camViewport.setPitch(-24.8f);
		}
		if (ImGui::RadioButton("place B", &p, 1)) {
			global::camViewport.setPos(glm::vec3(-9.35994, 9.0583, 11.9962));
			global::camViewport.setYaw(-49.2f);
			global::camViewport.setPitch(-9.99997f);
		}
		ImGui::Unindent();
	ImGui::Separator();

	ImGui::BulletText("Navigation mode");
		ImGui::Indent();
		ImGui::Text("record positions");
		if (!record_mode_enable) {
			if (ImGui::ArrowButton("record_button", ImGuiDir_Right)) {
				record_mode_enable = true;
				curve_points_file = new std::ofstream(
					"assets/data/camera_curve.txt");
			}
		}
		else {
			if (ImGui::Button("stop recording")) {
				record_mode_enable = false;
				curve_points_file->close();
			}
			ImGui::SameLine();
			if (ImGui::Button("add control point")) {
				glm::vec3 camPos = global::camViewport.getPos();

				if (curve_points_file->is_open()) {
					*curve_points_file << camPos[0] << " " 
						<< camPos[1] << " "
						<< camPos[2] << std::endl;
				}
				else {
					std::cout << "data file not open yet!" << std::endl;
				}
			}
		}
		ImGui::Text("curve navigation");
		if (!travel_mode_enable) {
			if (ImGui::ArrowButton("travel_button", ImGuiDir_Right)) {
				travel_mode_enable = true;

				global::travelTimer->start();
			}
		}
		else {
			if (ImGui::Button("stop")) {
				travel_mode_enable = false;

				global::travelTimer->pause();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("load curve")) {
			global::camViewport.loadControlPts(
				"assets/data/camera_curve.txt", CURVE_SAMPLES);
		}

		if (ImGui::SliderFloat("progress", &move_progress, 0, 1)) {
			if (global::camViewport.hasCurvePts()) {

				// set camera position
				std::vector<glm::vec3> &pts = global::camViewport.getCurvePts();
				int size = pts.size();
				int current_i = int((size - 1) * move_progress);
				glm::vec3 current_pos = pts[current_i];

				global::camViewport.setPos(current_pos);
			}
		}

		ImGui::Unindent();


	ImGui::End();
}

void weatherTool()
{

}

static float pitch_dir = 0;
static float yaw_dir = 0;
static float color_dir[3] = { 1.0, 1.0, 1.0 };
static float intensity_dir = 1.0;

static float pos_point[3] = { 0, 0, 0 };
static float color_point[3] = { 1.0, 1.0, 1.0 };
static float intensity_point = 1.0;
static float att_constant_point = 1.0;
static float att_linear_point = 0.5;
static float att_quadratic_point = 0.5;

static float pos_spot[3] = { 0, 0, 0 };
static float pitch_spot = 0;
static float yaw_spot = 0;
static float cutoff = 30;
static float outerCutoff = 35;
static float color_spot[3] = { 1.0, 1.0, 1.0 };
static float intensity_spot = 1.0;
static float att_constant_spot = 1.0;
static float att_linear_spot = 0.25;
static float att_quadratic_spot = 0.25;


void lightTool()
{
	ImGui::Begin("Light tool");

	ImGui::BulletText("Directional light setting");
		ImGui::Indent();

		if (ImGui::SliderFloat("pitch##dir", &pitch_dir, 0, 180)) {
			global::sun->setPitch(pitch_dir);

			global::program_model->bind();
			global::program_model->setUniformVec3("direcitonalLights[0].direction",
				global::sun->getDirection());
		}
		if (ImGui::SliderFloat("yaw##dir", &yaw_dir, 0, 360)) {
			global::sun->setYaw(yaw_dir);

			global::program_model->bind();
			global::program_model->setUniformVec3("direcitonalLights[0].direction",
				global::sun->getDirection());
		}
		if (ImGui::ColorEdit3("color##dir", color_dir)) {
			global::sun->setColor(glm::vec3(color_dir[0], color_dir[1], color_dir[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("direcitonalLights[0].color",
				global::sun->getColor());
		}
		if (ImGui::SliderFloat("intensity##dir", &intensity_dir, 0.0, 5.0)) {
			global::sun->setIntensity(intensity_dir);

			global::program_model->bind();
			global::program_model->setUniform1f("direcitonalLights[0].intensity",
				global::sun->getIntensity());
		}

		ImGui::Unindent();

	ImGui::BulletText("Point light setting");
		ImGui::Indent();

		if (ImGui::SliderFloat3("position##point", pos_point, -20, 20)) {
			global::pointLight->setTranslation(
				glm::vec3(pos_point[0], pos_point[1], pos_point[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("pointLights[0].position",
				global::pointLight->getPosition());
		}
		if (ImGui::ColorEdit3("color##point", color_point)) {
			global::pointLight->setColor(glm::vec3(color_point[0], color_point[1], color_point[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("pointLights[0].color",
				global::pointLight->getColor());
		}
		if (ImGui::SliderFloat("intensity##point", &intensity_point, 0.0, 5.0)) {
			global::pointLight->setIntensity(intensity_point);

			global::program_model->bind();
			global::program_model->setUniform1f("pointLights[0].intensity",
				global::pointLight->getIntensity());
		}
		if (ImGui::SliderFloat("attenuation(constant)##point", &att_constant_point, 1.0, 2.0)) {
			global::pointLight->setAttLinear(att_constant_point);

			global::program_model->bind();
			global::program_model->setUniform1f("pointLights[0].att_constant",
				global::pointLight->getAttConstant());
		}
		if (ImGui::SliderFloat("attenuation(linear)##point", &att_linear_point, 0.0, 3.0)) {
			global::pointLight->setAttLinear(att_linear_point);

			global::program_model->bind();
			global::program_model->setUniform1f("pointLights[0].att_linear",
				global::pointLight->getAttLinear());
		}
		if (ImGui::SliderFloat("attenuation(quadratic)##point", &att_quadratic_point, 0.0, 3.0)) {
			global::pointLight->setAttQuadratic(att_quadratic_point);

			global::program_model->bind();
			global::program_model->setUniform1f("pointLights[0].att_quadratic",
				global::pointLight->getAttQuadratic());
		}
		ImGui::Unindent();

	ImGui::BulletText("Spot light setting");
		ImGui::Indent();

		if (ImGui::SliderFloat3("position##spot", pos_spot, -20, 20)) {
			global::spotLight->setTranslation(
				glm::vec3(pos_spot[0], pos_spot[1], pos_spot[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("spotLights[0].position",
				global::spotLight->getPosition());
		}
		if (ImGui::SliderFloat("pitch##spot", &pitch_spot, 0, 180)) {
			global::spotLight->setPitch(pitch_spot);

			global::program_model->bind();
			global::program_model->setUniformVec3("spotLights[0].direction",
				global::spotLight->getDirection());
		}
		if (ImGui::SliderFloat("yaw##spot", &yaw_spot, 0, 360)) {
			global::spotLight->setYaw(yaw_spot);

			global::program_model->bind();
			global::program_model->setUniformVec3("spotLights[0].direction",
				global::spotLight->getDirection());
		}
		if (ImGui::SliderFloat("cutoff angle", &cutoff, 0, 180)) {
			global::spotLight->setCutoff(cutoff);

			global::program_model->bind();
			global::program_model->setUniform1f("spotLights[0].cutoff",
				cos(glm::radians(global::spotLight->getCutoff())));
		}
		if (ImGui::SliderFloat("outer cutoff angle", &outerCutoff, 0, 180)) {
			global::spotLight->setOuterCutoff(outerCutoff);

			global::program_model->bind();
			global::program_model->setUniform1f("spotLights[0].outerCutoff",
				cos(glm::radians(global::spotLight->getCutoff())));
		}
		if (ImGui::ColorEdit3("color##spot", color_spot)) {
			global::spotLight->setColor(glm::vec3(color_spot[0], color_spot[1], color_spot[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("spotLights[0].color",
				global::spotLight->getColor());
		}
		if (ImGui::SliderFloat("intensity##spot", &intensity_spot, 0.0, 5.0)) {
			global::spotLight->setIntensity(intensity_spot);

			global::program_model->bind();
			global::program_model->setUniform1f("spotLights[0].intensity",
				global::spotLight->getIntensity());
		}
		if (ImGui::SliderFloat("attenuation(constant)##spot", &att_constant_spot, 1.0, 2.0)) {
			global::spotLight->setAttConstant(att_constant_spot);

			global::program_model->bind();
			global::program_model->setUniform1f("spotLights[0].att_constant",
				global::spotLight->getAttConstant());
		}
		if (ImGui::SliderFloat("attenuation(linear)##spot", &att_linear_spot, 0.0, 3.0)) {
			global::spotLight->setAttLinear(att_linear_spot);

			global::program_model->bind();
			global::program_model->setUniform1f("spotLights[0].att_linear",
				global::spotLight->getAttLinear());
		}
		if (ImGui::SliderFloat("attenuation(quadratic)##spot", &att_quadratic_spot, 0.0, 3.0)) {
			global::spotLight->setAttQuadratic(att_quadratic_spot);

			global::program_model->bind();
			global::program_model->setUniform1f("spotLights[0].att_quadratic",
				global::spotLight->getAttQuadratic());
		}

		ImGui::Unindent();

	ImGui::End();
}


void nextCurvePts()
{
	move_progress += (float)(1.0 / CURVE_SAMPLES);
	if (move_progress > 1.0)
		move_progress = 0;

	// set camera position
	std::vector<glm::vec3>& pts = global::camViewport.getCurvePts();
	int size = pts.size();
	int current_i = int((size - 1) * move_progress);
	glm::vec3 current_pos = pts[current_i];

	global::camViewport.setPos(current_pos);
}