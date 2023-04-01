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
	BackGround = glm::vec3(56.0 / 255.0);

	light = std::make_shared<DirectionalLight>(glm::vec3(20, 50, 20), glm::vec3(255 / 256.0, 244 / 256.0, 214 / 256.0));
	lightShader = std::make_shared<Shader>("shader/light/light.vs", "shader/light/light.fs");

	TexMap.emplace("SkyBoxMap", LoadSkyBox("image/hdr/newport_loft.hdr"));
	
	ChartletMap.emplace("White", LoadTexture("image/white.jpg"));
	ChartletMap.emplace("DirectionLight", LoadTexture("image/DirectionLight.png"));
	ChartletMap.emplace("PointLight", LoadTexture("image/PointLight.png"));
	ChartletMap.emplace("rusted_iron_albedo", LoadTexture("image/pbr/rusted_iron/rusted_iron_albedo.png"));
	ChartletMap.emplace("rusted_iron_normal", LoadTexture("image/pbr/rusted_iron/rusted_iron_normal.png"));
	ChartletMap.emplace("rusted_iron_metallic", LoadTexture("image/pbr/rusted_iron/rusted_iron_metallic.png"));
	ChartletMap.emplace("rusted_iron_roughness", LoadTexture("image/pbr/rusted_iron/rusted_iron_roughness.png"));
	ChartletMap.emplace("rusted_iron_ao", LoadTexture("image/pbr/rusted_iron/rusted_iron_ao.png"));
	ChartletMap.emplace("None_Tex", 0);

	objs = std::make_shared<Objects>();
	std::shared_ptr<Object> sphere = std::make_shared<Sphere>("Sphere" + std::to_string(objs->get_objs().size()), "BlinPhone");
	objs->add(sphere);
	std::shared_ptr<Object> plane = std::make_shared<Plane>("Plane" + std::to_string(objs->get_objs().size()), "BlinPhone");
	objs->add(plane);

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

	 //debug
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, scr_WIDTH, scr_HEIGHT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Debugshader->use();
	//Debugshader->setInt("Tex", 0);
	//Debugshader->setInt("Tex2", 1);
	//Debugshader->setMat4("view", view);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, TexMap.at("SSR"));
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, TexMap.at("ForwardID"));
	//renderQuad();
}

void Program::Update() {
}

#endif // !PROGRAM_H
