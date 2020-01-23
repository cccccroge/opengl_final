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
				show_fps = !show_fps;
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

	if (show_fps) {
		ImGui::SetNextWindowBgAlpha(1);
		ImGui::Begin("FPS");

		std::string fpsStr = std::to_string(global::current_fps);
		ImGui::Text(fpsStr.c_str());

		ImGui::End();
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
		if (ImGui::RadioButton("Monkey Head", &p, 0)) {
			global::camViewport.setPos(glm::vec3(1.405, 6.92, 2.97));
			global::camViewport.setYaw(-88.0f);
			global::camViewport.setPitch(-46.6f);
		}
		if (ImGui::RadioButton("King", &p, 1)) {
			global::camViewport.setPos(glm::vec3(6.74, 3.95, 5.64));
			global::camViewport.setYaw(-172.6f);
			global::camViewport.setPitch(-2.2f);
		}
		if (ImGui::RadioButton("Cabin", &p, 2)) {
			global::camViewport.setPos(glm::vec3(-14.0802, -1.5759, 27.8747));
			global::camViewport.setYaw(-332.4);
			global::camViewport.setPitch(-1.6);
		}
		if (ImGui::RadioButton("Island", &p, 3)) {
			global::camViewport.setPos(glm::vec3(39.2144, -2.09475, 18.87987));
			global::camViewport.setYaw(-168.8);
			global::camViewport.setPitch(14.2);
		}
		if (ImGui::RadioButton("Mouse Family", &p, 4)) {
			global::camViewport.setPos(glm::vec3(17.2529, -0.39504, 17.3588));
			global::camViewport.setYaw(-162.6);
			global::camViewport.setPitch(6.0);
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


static bool check = true;
static bool check2 = true;
bool snow_effect_enable = true;
bool ocean_effect_enable = true;
static float gravity = 1.5;
static float density = 2.5;
static float wind_speed[2] = { 3., 3. };

static float color_fog[3] = { 209 / 255.0, 209 / 255.0, 209 / 255.0 };
static float density_fog = 0.03;

static float water_pos[3] = { 0., 0., 0. };

void weatherTool()
{
	ImGui::Begin("Weather tool");

	ImGui::BulletText("Snow particles");
		ImGui::Indent();

		if (ImGui::Checkbox("enable", &check)) {
			snow_effect_enable = !snow_effect_enable;
		}

		if (ImGui::SliderFloat("gravity", &gravity, 0.1, 10)) {
			global::snowEffectObj->setGravity(gravity);
		}
		if (ImGui::SliderFloat("density", &density, 1, 3)) {
			global::snowEffectObj->setDensity(density);
		}
		if (ImGui::SliderFloat2("wind speed(x, z)", wind_speed, -10, 10)) {
			global::snowEffectObj->setWindSpeed(
				glm::vec2(wind_speed[0], wind_speed[1]));
		}

		ImGui::Unindent();

	ImGui::BulletText("Fog");
		ImGui::Indent();

		if (ImGui::ColorEdit3("color##fog", color_fog)) {
			glm::vec3 fogColor = glm::vec3(color_fog[0], color_fog[1], color_fog[2]);

			global::program_model->bind();
			global::program_model->setUniformVec3("fogColor", fogColor);
		}
		if (ImGui::SliderFloat("density##fog", &density_fog, 0.005, 0.1)) {
			global::program_model->bind();
			global::program_model->setUniform1f("fogDensity", density_fog);

			global::waterEffectObj->program_ocean->setUniform1f("fogDensity", density_fog);
		}
		ImGui::Unindent();


	ImGui::BulletText("Ocean");
		ImGui::Indent();

		if (ImGui::Checkbox("enable##ocean", &check2)) {
			ocean_effect_enable = !ocean_effect_enable;
		}

		ImGui::Unindent();


	ImGui::End();
}

static double bias_base = 0.00005;
static double bias_base_min = 0.00001;
static double bias_base_max = 0.00025;
static double bias_factor = 0.0005;
static double bias_factor_min = 0.0001;
static double bias_factor_max = 0.0025;

static float pos_dir[3] = { 35, 20, 3 };
static float near_dir = 0.1f;
static float far_dir = 150.0f;
static float pitch_dir = 45;
static float yaw_dir = 87;
static float color_dir[3] = { 240/255.0, 228/255.0, 199/255.0 };
static float intensity_dir = 1;

static float pos_point[3] = { 1.592, 7.707, 2.381 };
static float color_point[3] = { 150 / 255.0, 140 / 255.0, 1.0 };
static float intensity_point = 5.0;
static float att_constant_point = 1.0;
static float att_linear_point = 0.5;
static float att_quadratic_point = 0.2;

static float pos_spot[3] = { 6.18, 4.45, 5.54 };
static float pitch_spot = 26;
static float yaw_spot = 7;
static float cutoff = 20;
static float outerCutoff = 25;
static float color_spot[3] = { 227 / 255.0, 144 / 255.0, 185 / 255.0 };
static float intensity_spot = 1.0;
static float att_constant_spot = 1.0;
static float att_linear_spot = 0.25;
static float att_quadratic_spot = 0.25;


void lightTool()
{
	ImGui::Begin("Light tool");

	ImGui::BulletText("Shadow bias");
		ImGui::Indent();
		
		if (ImGui::DragScalar("base", ImGuiDataType_Double, &bias_base, 0.00001f,
			&bias_base_min, &bias_base_max)) {
			global::program_model->bind();
			global::program_model->setUniform1d("BIAS_BASE", bias_base);
		}
		if (ImGui::DragScalar("factor", ImGuiDataType_Double, &bias_factor, 0.0001f,
			&bias_factor_min, &bias_factor_max)) {
			global::program_model->bind();
			global::program_model->setUniform1d("BIAS_FACTOR", bias_factor);
		}

		ImGui::Unindent();


	ImGui::BulletText("Directional light setting");
		ImGui::Indent();

		if (ImGui::SliderFloat3("position##dir", pos_dir, -100, 100)) {
			global::sun->setTranslation(
				glm::vec3(pos_dir[0], pos_dir[1], pos_dir[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("direcitonalLights[0].position",
				global::sun->getPosition());
		}
		if (ImGui::SliderFloat("near##dir", &near_dir, 0, 10)) {
			global::sun->setNear(near_dir);
		}
		if (ImGui::SliderFloat("far##dir", &far_dir, 0, 1000)) {
			global::sun->setFar(far_dir);
		}
		if (ImGui::SliderFloat("pitch##dir", &pitch_dir, 1, 179)) {
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

			global::waterEffectObj->program_ocean->bind();
			global::waterEffectObj->program_ocean->setUniform1f("sunIntensity", global::sun->getIntensity());
		}

		ImGui::Unindent();

	ImGui::BulletText("Point light setting");
		ImGui::Indent();

		if (ImGui::SliderFloat3("position##point", pos_point, -10, 10)) {
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
		if (ImGui::SliderFloat("intensity##point", &intensity_point, 0.0, 10.0)) {
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

		if (ImGui::SliderFloat3("position##spot", pos_spot, 0, 15)) {
			global::spotLight->setTranslation(
				glm::vec3(pos_spot[0], pos_spot[1], pos_spot[2]));

			global::program_model->bind();
			global::program_model->setUniformVec3("spotLights[0].position",
				global::spotLight->getPosition());
		}
		if (ImGui::SliderFloat("pitch##spot", &pitch_spot, 1, 179)) {
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


void calTime()
{
	currentTime = currentTime + 20;
}