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

static float pos[3] = { 0, 0, 0 };
static float pitch = 0;
static float yaw = 0;
static float intensity = 1.0;
static float color[3] = { 1.0, 1.0, 1.0 };
static float cutoff = 15.0;
static float outerCutoff = 18.0f;

void lightTool()
{
	ImGui::Begin("Light tool");

	ImGui::BulletText("Directional light setting");
		ImGui::Indent();

		if (ImGui::SliderFloat("pitch", &pitch, 0, 180)) {
			global::sun->setPitch(pitch);

			global::program_model->bind();
			global::program_model->setUniformVec3("direcitonalLights[0].direction",
				global::sun->getDirection());
		}
		if (ImGui::SliderFloat("yaw", &yaw, 0, 360)) {
			global::sun->setYaw(yaw);

			global::program_model->bind();
			global::program_model->setUniformVec3("direcitonalLights[0].direction",
				global::sun->getDirection());
		}
		if (ImGui::SliderFloat("intensity", &intensity, 0.0, 5.0)) {
			global::sun->setIntensity(intensity);

			global::program_model->bind();
			global::program_model->setUniform1f("direcitonalLights[0].intensity",
				global::sun->getIntensity());
		}
		if (ImGui::ColorEdit3("color", color)) {
			global::sun->setColor(glm::vec3(color[0], color[1], color[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("direcitonalLights[0].color",
				global::sun->getColor());
		}

		/*static int m = 0;
		if (ImGui::RadioButton("directional light", &m, 0)) {
			global::program_model->bind();
			global::program_model->setUniform1i("lightMode", 0);
		}
		if (ImGui::RadioButton("point light", &m, 1)) {
			global::program_model->bind();
			global::program_model->setUniform1i("lightMode", 1);
		}
		if (ImGui::RadioButton("spot light", &m, 2)) {
			global::program_model->bind();
			global::program_model->setUniform1i("lightMode", 2);
		}

		if (ImGui::SliderFloat3("position", pos, -20, 20)) {
			global::camLight.setPos(glm::vec3(pos[0], pos[1], pos[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("lightPos", 
				global::camLight.getPos());
		}
		if (ImGui::SliderFloat("pitch", &pitch, -89, 89)) {
			global::camLight.setPitch(pitch);

			global::program_model->bind();
			global::program_model->setUniformVec3("lightDir",
				global::camLight.getDirection('f'));
		}
		if (ImGui::SliderFloat("yaw", &yaw, 0, 360)) {
			global::camLight.setYaw(yaw);

			global::program_model->bind();
			global::program_model->setUniformVec3("lightDir",
				global::camLight.getDirection('f'));
		}
		if (ImGui::SliderFloat("cufoff angle", &cutoff, 0, 180)) {
			global::program_model->bind();
			global::program_model->setUniform1f("lightCutoff", cos(glm::radians(cutoff)));
		}
		if (ImGui::SliderFloat("outer cufoff angle", &outerCutoff, 0, 180)) {
			global::program_model->bind();
			global::program_model->setUniform1f("lightOuterCutoff", cos(glm::radians(outerCutoff)));
		}*/

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