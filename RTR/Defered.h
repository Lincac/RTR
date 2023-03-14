#pragma once

#ifndef DEFERED_H
#define DEFERED_H

#include"RenderPath.h"

class Defered : public RenderPath {
public:
	Defered(std::shared_ptr<RenderMode> mode);
	~Defered();
	virtual unsigned int Render(std::shared_ptr<Objects> objs) override;
private:
	unsigned int FBO;
	unsigned int textureID;
	bool OpenSSAO;
	bool OpenSSR;
	bool OpenTAA;
	std::shared_ptr<RenderMode> renderMode;
protected:
	unsigned int HDRFBO;
	unsigned int HDRtextureID;
	virtual void HDR(std::shared_ptr<Objects> objs) override;
};

Defered::Defered(std::shared_ptr<RenderMode> mode) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::DWWidth, Window::DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Forawrd Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &HDRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);

	glGenTextures(1, &HDRtextureID);
	glBindTexture(GL_TEXTURE_2D, HDRtextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::DWWidth, Window::DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HDRtextureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Forward HDR Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	OpenSSAO = false;
	OpenSSR = false;
	OpenTAA = false;

	renderMode = mode;
}

Defered::~Defered() {
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &textureID);
	glDeleteFramebuffers(1, &HDRFBO);
	glDeleteTextures(1,&HDRtextureID);
}

unsigned int Defered::Render(std::shared_ptr<Objects> objs) {
	// gbuffer
	std::shared_ptr<RenderHelp> gbuffer = passes.at("gbuffer");
	gbuffer->RenderPass(objs, renderMode->getRenderModeName());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClearColor(BackGround.x, BackGround.y, BackGround.z, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	renderMode->DRender();

	// ��ȡ gbuffer �����
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->GetPassFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // д�뵽Ĭ��֡����
	glBlitFramebuffer(0, 0, Window::DWWidth, Window::DWHeight, 0, 0, Window::DWWidth, Window::DWHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	light->RenderLight(view, projection, camera.Position);

	if (OpenSkyBox)
	{
		skyboxshader->use();
		skyboxshader->setInt("cubemap", 0);
		skyboxshader->setMat4("view", view);
		skyboxshader->setMat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TexMap.at("SkyBoxMap"));
		renderCube();
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO); // д�뵽Ĭ��֡����
	glBlitFramebuffer(0, 0, Window::DWWidth, Window::DWHeight, 0, 0, Window::DWWidth, Window::DWHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::scr_WIDTH, Window::scr_HEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (OpenHDR) {
		HDR(objs);
		return HDRtextureID;
	}
	else
		return textureID;
}

void Defered::HDR(std::shared_ptr<Objects> objs) {
	if (OpenSSAO)
	{
		std::shared_ptr<RenderHelp> ssao = passes.at("SSAOPass");
		ssao->RenderPass(objs);
	}

	if (OpenSSR)
	{
		std::shared_ptr<RenderHelp> gbuffer = passes.at("GBufferPass");
		gbuffer->RenderPass(objs);
		std::shared_ptr<RenderHelp> ssao = passes.at("SSAOPass");
		ssao->RenderPass(objs);
	}

	if (OpenTAA && OpenSSR)
	{
		std::shared_ptr<RenderHelp> taa = passes.at("SSAOPass");
		taa->RenderPass(objs);
	}
	else {
		std::shared_ptr<RenderHelp> gbuffer = passes.at("GBufferPass");
		gbuffer->RenderPass(objs);
		std::shared_ptr<RenderHelp> taa = passes.at("SSAOPass");
		taa->RenderPass(objs);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	processshader->use();
	processshader->setInt("HDR", 0);
	processshader->setInt("SSAO", 1);
	processshader->setInt("SSR", 2);
	processshader->setFloat("exposure", exposure);
	processshader->setBool("OpenSSAO", OpenSSAO);
	processshader->setBool("OpenSSR", OpenSSR);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("SSAO"));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("SSR"));
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
#endif // !DEFERED_H
