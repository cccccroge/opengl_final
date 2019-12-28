#include "GLEW/glew.h"
#include "imGui/imgui.h"
#include "imGui/imgui_impl_glut.h"
#include "imGui/imgui_impl_opengl3.h"
#include "freeGLUT/freeglut.h"

#include "GLM/vec3.hpp"

#include "global.h"
#include "utils.h"
#include "init.h"
#include "scene/Model.h"
#include "render/Shader.h"
#include "render/ShaderProgram.h"
#include "scene/Camera.h"
#include "render/Renderer.h"
#include "render/PostEffectBuffer.h"
#include "event/timer.h"
#include "event/gui.h"

#include <iostream>

ShaderProgram* global::program_model;
ShaderProgram* global::program_posteffect;
ShaderProgram* global::program_skybox;
ShaderProgram* global::program_shadow;
PostEffectBuffer* global::postEffectBuffer;
FrameBuffer* global::depthMapBuffer;
Texture* global::depthTex;
Renderer* global::renderer;
Model* global::Man;
Skybox* global::skybox;
Timer* global::travelTimer;


void setupRendering()
{
	// setup shader program
	Shader vertexShader_first = Shader(GL_VERTEX_SHADER, "assets/shader/model.vs.glsl");
	Shader fragmentShader_first = Shader(GL_FRAGMENT_SHADER, "assets/shader/model.fs.glsl");
	global::program_model = new ShaderProgram();
	global::program_model->addShader(vertexShader_first);
	global::program_model->addShader(fragmentShader_first);
	global::program_model->compile();

	Shader vertexShader_second = Shader(GL_VERTEX_SHADER, "assets/shader/posteffect.vs.glsl");
	Shader fragmentShader_second = Shader(GL_FRAGMENT_SHADER, "assets/shader/posteffect.fs.glsl");
	global::program_posteffect = new ShaderProgram();
	global::program_posteffect->addShader(vertexShader_second);
	global::program_posteffect->addShader(fragmentShader_second);
	global::program_posteffect->compile();

	Shader vertexShader_skybox = Shader(GL_VERTEX_SHADER, "assets/shader/skybox.vs.glsl");
	Shader fragmentShader_skybox = Shader(GL_FRAGMENT_SHADER, "assets/shader/skybox.fs.glsl");
	global::program_skybox = new ShaderProgram();
	global::program_skybox->addShader(vertexShader_skybox);
	global::program_skybox->addShader(fragmentShader_skybox);
	global::program_skybox->compile();

	Shader vertexShader_shadow = Shader(GL_VERTEX_SHADER, "assets/shader/shadow.vs.glsl");
	Shader fragmentShader_shadow = Shader(GL_FRAGMENT_SHADER, "assets/shader/shadow.fs.glsl");
	global::program_shadow = new ShaderProgram();
	global::program_shadow->addShader(vertexShader_shadow);
	global::program_shadow->addShader(fragmentShader_shadow);
	global::program_shadow->compile();
   
	// setup models
	global::Man = new Model("assets/model/low_poly_winter_scene/Low Poly Winter Scene.obj");

	// setup skybox
	global::skybox = new Skybox({
		"assets/image/cubemap_forest/posx.png",
		"assets/image/cubemap_forest/negx.png",
		"assets/image/cubemap_forest/posy.png",
		"assets/image/cubemap_forest/negy.png",
		"assets/image/cubemap_forest/posz.png",
		"assets/image/cubemap_forest/negz.png",
	});
	global::skybox->scale(1.0f, 1.0f, -1.0f);	// flip it for godsake!

	// setup camera
	global::camViewport = Camera(PROJECTION_TYPE::PERSPECTIVE, 	// main camera
		std::vector<float>({ 0.1f, 1000.0f }), glm::vec3(0.0f, 15.0f, 0.0f), 
		0, 0, 80.0f);
	global::camLight = Camera(PROJECTION_TYPE::ORTHOGONAL,	// light camera to produce depth map
		std::vector<float>({ 0.1f, 500.0f, -10.0f, 10.0f, -10.0f, 10.0f }), 
		glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, 0.0f, 0);

	// send to renderer
	global::renderer = new Renderer();
	global::renderer->addModel(*global::Man);
	global::renderer->addSkybox(*global::skybox);
	global::renderer->setMainCamera(global::camViewport);
	global::renderer->setLightCamera(global::camLight);

	// set up post effect buffer
	global::postEffectBuffer = new PostEffectBuffer(MAINWINDOW_WIDTH,
		MAINWINDOW_HEIGHT);
	
	global::program_posteffect->bind();
	global::program_posteffect->setUniform1i("screenTex", 0);

	// set up depth map buffer
	global::depthMapBuffer = new FrameBuffer();
	global::depthTex = new Texture(0, 1024, 1024);
	global::depthMapBuffer->attachTexture(*global::depthTex, GL_DEPTH_ATTACHMENT);
	global::depthMapBuffer->attachEmptyColorBuffer();
	global::depthMapBuffer->validate();

	// set up uniforms in first program
	global::program_model->bind();
	global::program_model->setUniform1i("lightMode", 0);	// default: directional light
	global::program_model->setUniformVec3("lightPos", 
		global::camLight.getPos());
	global::program_model->setUniformVec3("lightDir",
		global::camLight.getDirection('f'));
	global::program_model->setUniform1f("lightCutoff", cos(glm::radians(15.0f)));
	global::program_model->setUniform1f("lightOuterCutoff", cos(glm::radians(18.0f)));
	
	global::program_model->setUniform1f("ambientStrength", 0.0f);
	global::program_model->setUniformVec3("ambientAlbedo",
		glm::vec3(1.0f, 1.0f, 1.0f));
	global::program_model->setUniformVec3("specularAlbedo",
		glm::vec3(0.7f, 0.7f, 0.7f));
	global::program_model->setUniform1i("specularPower", 200);

	// set up navigation travel tool timer
	global::travelTimer = new Timer(TIMER_TYPE::REPEAT, 5, nextCurvePts);
}


int main(int argc, char *argv[])
{
	/* Glut initialization */
	initGlutContext(argc, argv);
	createMenu();
	registerCallbacks();
	global::timer.Start();
	
	/* ImGui initialization */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs();
	ImGui_ImplOpenGL3_Init();

	float SCALE = 1.25f;
	ImFontConfig cfg;
	cfg.SizePixels = 13 * SCALE;
	ImGui::GetIO().Fonts->AddFontDefault(&cfg)->DisplayOffset.y = SCALE;

	/* GLEW initialization */
	initGlew();
	dumpInfo();

	/* Render initialization */
	setupRendering();

	/* Enter main event loop */
	glutMainLoop();	

	// MainLoop ends here, free mem.
	std::cout << "Return control from main loop." << std::endl;
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();

	delete global::postEffectBuffer;
	delete global::renderer;
	delete global::Man;
	delete global::program_posteffect;
	delete global::program_model;
	
	return 0;
}