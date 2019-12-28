#include "gui.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glut.h"
#include "imGui/imgui_impl_opengl3.h"
#include "../utils.h"
#include "../global.h"
#include <iostream>
#include <fstream> 

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

		ImGui::Unindent();


	ImGui::End();
}

void weatherTool()
{

}

static float pos[3] = { 0, 0, 0 };
static float angle = 45;
static float yaw = 0;

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
	if (ImGui::SliderFloat3("position", pos, -10, 10)) {
		global::camLight.setPos(glm::vec3(pos[0], pos[1], pos[2]));

		global::program_model->bind();
		global::program_model->setUniformVec3("lightPos", 
			global::camLight.getPos());
	}
	if (ImGui::SliderFloat("pitch", &angle, -89, 89)) {
		global::camLight.setPitch(angle);

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

	ImGui::Unindent();


	ImGui::End();
}