#include "gui.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glut.h"
#include "imGui/imgui_impl_opengl3.h"
#include "../utils.h"
#include "../global.h"

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
			global::camViewport.setPos(glm::vec3(0, 3.0, 0));
		}
		if (ImGui::RadioButton("place B", &p, 1)) {

		}
		ImGui::Unindent();
	ImGui::Separator();

	ImGui::BulletText("Navigation mode");


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

	ImGui::BulletText("Sun setting");
	ImGui::Indent();
	if (ImGui::SliderFloat3("position", pos, -300, 300)) {
		global::camLight.setPos(glm::vec3(pos[0], pos[1], pos[2]));
	}
	if (ImGui::SliderFloat("pitch", &angle, -89, 89)) {
		global::camLight.setPitch(angle);
	}
	if (ImGui::SliderFloat("yaw", &yaw, 0, 360)) {
		global::camLight.setYaw(yaw);
	}

	ImGui::Unindent();


	ImGui::End();
}