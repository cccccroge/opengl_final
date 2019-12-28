#include "gui.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glut.h"
#include "imGui/imgui_impl_opengl3.h"
#include "../utils.h"
#include "../global.h"
#include <iostream>
#include <fstream> 
#include "GLM/trigonometric.hpp"

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
static float move_progress = 0.0;

void cameraTool()
{
	ImGui::Begin("Camera tool");

	ImGui::BulletText("Camera setting");
		ImGui::Indent();
		if (ImGui::SliderFloat("fov", &fov, 30.0f, 150.0f)) {
			global::camViewport.setFov(fov);
		}
		ImGui::Unindent();
	ImGui::Separator();

	ImGui::BulletText("Static mode");
		ImGui::Indent();
		static int p = 0;
		if (ImGui::RadioButton("place A", &p, 0)) {
			global::camViewport.setPos(glm::vec3(3.44579, 11.9493, 3.00064));
			global::camViewport.setYaw(-114.4);
			global::camViewport.setPitch(-24.8);
		}
		if (ImGui::RadioButton("place B", &p, 1)) {
			global::camViewport.setPos(glm::vec3(-9.35994, 9.0583, 11.9962));
			global::camViewport.setYaw(-49.2);
			global::camViewport.setPitch(-9.99997);
		}
		ImGui::Unindent();
	ImGui::Separator();

	ImGui::BulletText("Navigation mode");
		ImGui::Indent();
		ImGui::Text("record positions");
		if (!record_mode_enable) {
			if (ImGui::ArrowButton("arrow button", ImGuiDir_Right)) {
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
		if (ImGui::Button("load curve")) {
			global::camViewport.loadControlPts(
				"assets/data/camera_curve.txt", 100);
		}

		if (ImGui::SliderFloat("move", &move_progress, 0, 1)) {
			if (global::camViewport.hasCurvePts()) {

				// set camera position
				std::vector<glm::vec3> &pts = global::camViewport.getCurvePts();
				int size = pts.size();
				int current_i = int((size - 1) * move_progress);
				glm::vec3 current_pos = pts[current_i];

				global::camViewport.setPos(current_pos);

				// set pitch and yaw according to direction
				int next_i = (current_i + 1) % size;
				glm::vec3 next_pos = pts[next_i];
				glm::vec3 direction = glm::normalize(next_pos - current_pos);
				float pitch = glm::degrees(asin(direction.y));
				float yaw = glm::degrees(atan2(direction.x, direction.z))+180;

				global::camViewport.setPitch(pitch);
				global::camViewport.setYaw(yaw);	// what is the correct way?
			}
		}

		ImGui::Unindent();


	ImGui::End();
}

void weatherTool()
{

}

static float pos[3] = { 0, 0, 0 };
static float pitch = 45;
static float yaw = 0;
static float cutoff = 15.0;
static float outerCutoff = 18.0f;

void lightTool()
{
	ImGui::Begin("Light tool");

	ImGui::BulletText("Light setting");
		ImGui::Indent();

		static int m = 0;
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
		}

		ImGui::Unindent();


	ImGui::End();
}