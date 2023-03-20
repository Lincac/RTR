#pragma once

#ifndef FORWARD_H
#define FORWARD_H

#include"RenderPath.h"

class Forward : public RenderPath {
public:
	Forward(std::shared_ptr<RenderMode> mode);
	~Forward();
	virtual unsigned int Render(std::shared_ptr<Objects> objs) override;
private:
	unsigned int FBO;
	unsigned int textureID;
	std::shared_ptr<RenderMode> renderMode;
protected:
	unsigned int HDRFBO;
	unsigned int HDRtextureID;  
	virtual void HDR(std::shared_ptr<Objects> objs) override; 
};
Forward::Forward(std::shared_ptr<RenderMode> mode) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::DWWidth, Window::DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, Window::DWWidth, Window::DWHeight, GL_TRUE);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureID, 0);
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

	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, HDRtextureID);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, Window::DWWidth, Window::DWHeight, GL_TRUE);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, HDRtextureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Forward HDR Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	renderMode = mode;
}

Forward::~Forward() {
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &textureID);
	glDeleteFramebuffers(1, &HDRFBO);
	glDeleteTextures(1, &HDRtextureID);
}

unsigned int Forward::Render(std::shared_ptr<Objects> objs) {
	if (OpenSSR && renderMode->getRenderModeName() == "BlinPhone")
	{
		std::shared_ptr<RenderHelp> gbuffer = passes.at("gbuffer");
		gbuffer->RenderPass(objs, renderMode->getRenderModeName());
		std::shared_ptr<RenderHelp> ssr = passes.at("ssr");
		ssr->RenderPass(objs);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClearColor(BackGround.x, BackGround.y, BackGround.z, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_MULTISAMPLE);

	renderMode->Render(objs);

	light->RenderLight(view,projection,camera.Position);

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
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO); // –¥»ÎµΩƒ¨»œ÷°ª∫≥Â
	glBlitFramebuffer(0, 0, Window::DWWidth, Window::DWHeight, 0, 0, Window::DWWidth, Window::DWHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	HDR(objs);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::scr_WIDTH, Window::scr_HEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	return HDRtextureID;
}

void Forward::HDR(std::shared_ptr<Objects> objs) {
	glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_MULTISAMPLE);

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

#endif // !FORWARD_H
