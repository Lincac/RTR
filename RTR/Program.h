 #pragma once

#ifndef PROGRAM_H
#define PROGRAM_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<vector>

#include"OperateWindow.h"
#include"ObjectWindow.h"
#include"DrawWindow.h"

class Program {
public:
	Program() {};
	Program(GLFWwindow* w) {
		window = w;
	}
	void Init();
	void Render();
	void Update();
private:
	GLFWwindow* window;
	std::shared_ptr<Window> objectwindow;
	std::shared_ptr<Window> operatewindow;
	std::shared_ptr<Window> drawwindow;

	// load objs
	std::shared_ptr<Objects> objs;
};

void Program::Init() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	objectwindow = std::make_shared<ObjectWindow>();
	operatewindow = std::make_shared<OperateWindow>();
	drawwindow = std::make_shared<DrawWindow>();

	Ambient = glm::vec3(0.03);
	BackGround = glm::vec3(56.0 / 256.0);

	light = std::make_shared<DirectionalLight>(glm::vec3(20, 50, 20), glm::vec3(255 / 256.0, 244 / 256.0, 214 / 256.0));
	lightShader = std::make_shared<Shader>("shader/light/light.vs", "shader/light/light.fs");
	
	unsigned int skyboxmap = LoadSkyBox("image/hdr/newport_loft.hdr");
	TexMap.emplace("SkyBoxMap", skyboxmap);
	unsigned int white = LoadTexture("image/white.jpg");
	ChartletMap.emplace("White", white);
	unsigned int Dlight = LoadTexture("image/DirectionLight.png");
	ChartletMap.emplace("DirectionLight", Dlight);
	unsigned int Plight = LoadTexture("image/PointLight.png");
	ChartletMap.emplace("PointLight", Plight);

	objs = std::make_shared<Objects>();
	std::shared_ptr<Object> sphere = std::make_shared<Sphere>("Sphere" + std::to_string(objs->get_objs().size()), "PBR");
	objs->add(sphere);

	std::shared_ptr<RenderHelp> csm = std::make_shared<CSMPass>();
	passes.emplace("csm", csm);
	std::shared_ptr<RenderHelp> gbuffer = std::make_shared<GBufferPass>();
	passes.emplace("gbuffer", gbuffer);
	std::shared_ptr<RenderHelp> irradiance = std::make_shared<IrradiancePass>();
	passes.emplace("irradiance", irradiance);
	std::shared_ptr<RenderHelp> lutpass = std::make_shared<LUTPass>();
	passes.emplace("lutpass", lutpass);
	std::shared_ptr<RenderHelp> prefilter = std::make_shared<PrefilterPass>();
	passes.emplace("prefilter", prefilter);
	std::shared_ptr<RenderHelp> ssao = std::make_shared<SSAOPass>();
	passes.emplace("ssao", ssao);
	std::shared_ptr<RenderHelp> ssr = std::make_shared<SSRPass>();
	passes.emplace("ssr", ssr);

	skyboxshader = std::make_shared<Shader>("shader/skybox/sky.vs", "shader/skybox/sky.fs");
	Debugshader = std::make_shared<Shader>("shader/temp/temp.vs", "shader/temp/temp.fs");
	processshader = std::make_shared<Shader>("shader/hdr/hdr.vs", "shader/hdr/hdr.fs");
}

void Program::Render() {
	 //csm
	std::shared_ptr<RenderHelp> csm = passes.at("csm");
	csm->RenderPass(objs);

	//// ssao 
	if (OpenSSAO)
	{
		std::shared_ptr<RenderHelp> gbuffer = passes.at("gbuffer");
		gbuffer->RenderPass(objs);
		std::shared_ptr<RenderHelp> ssao = passes.at("ssao");
		ssao->RenderPass(objs);
	}

	std::shared_ptr<RenderPath> renderPath = drawwindow->GetRenderPath();
	unsigned int ID = renderPath->Render(objs);

	ImGui_ImplOpenGL3_NewFrame();	
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	objectwindow->RenderWindow(objs);
	drawwindow->RenderWindow(objs,ID);
	operatewindow->RenderWindow(objs);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	Update();

	 //debug
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, scr_WIDTH, scr_HEIGHT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Debugshader->use();
	//Debugshader->setInt("Tex", 0);
	//Debugshader->setInt("Tex2", 1);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, TexMap.at("SSR"));
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, TexMap.at("gAlbedo"));
	//renderQuad();
}

void Program::Update() {
}

#endif // !PROGRAM_H
