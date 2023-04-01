#pragma once

#ifndef DEFERED_H
#define DEFERED_H

#include"RenderPath.h"
#include"TAAPass.h"

class Defered : public RenderPath {
public:
	Defered(std::shared_ptr<RenderMode> mode);
	~Defered();
	virtual unsigned int Render(std::shared_ptr<Objects> objs) override;
	virtual void SetRenderMode(std::shared_ptr<RenderMode> rm) override { renderMode = rm; };
	virtual std::string getRenderPathName() override { return "Defered"; };
private:
	unsigned int FBO;
	unsigned int textureID;
	std::shared_ptr<RenderMode> renderMode;
protected:
	unsigned int HDRFBO;
	unsigned int HDRtextureID;
	virtual void HDR(std::shared_ptr<Objects> objs) override;
	std::shared_ptr<TAAPass> taa;
};

Defered::Defered(std::shared_ptr<RenderMode> mode) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DWWidth, DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Forawrd Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &HDRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);

	glGenTextures(1, &HDRtextureID);
	glBindTexture(GL_TEXTURE_2D, HDRtextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, DWWidth, DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HDRtextureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Forward HDR Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TexMap.emplace("HDR",HDRtextureID);

	renderMode = mode;
	taa = std::make_shared<TAAPass>();
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

	// defered shading
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, DWWidth, DWHeight);
	glClearColor(BackGround.x, BackGround.y, BackGround.z, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderMode->DRender();

	// 获取 gbuffer 的深度
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer->GetPassFBO());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入到默认帧缓冲
	glBlitFramebuffer(0, 0, DWWidth, DWHeight, 0, 0, DWWidth, DWHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

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
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO); // 写入到默认帧缓冲
	glBlitFramebuffer(0, 0, DWWidth, DWHeight, 0, 0, DWWidth, DWHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// taa
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, DWWidth, DWHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	taa->shader->use();
	taa->shader->setInt("currentcolor",0);
	taa->shader->setInt("historycolor", 0);
	taa->shader->setInt("gVelo", 0);
	taa->shader->setInt("gPosition", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, taa->historycolor);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gVelo"));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gPosition"));
	renderQuad();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, taa->historyframe);
	glBlitFramebuffer(0, 0, DWWidth, DWHeight, 0, 0, DWWidth, DWHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (OpenSSR)
	{
		std::shared_ptr<RenderHelp> ssr = passes.at("ssr");
		ssr->RenderPass(objs);
	}

	HDR(objs);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, scr_WIDTH, scr_HEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	return HDRtextureID;
}

void Defered::HDR(std::shared_ptr<Objects> objs) {
	glBindFramebuffer(GL_FRAMEBUFFER, HDRFBO);
	glViewport(0, 0, DWWidth, DWHeight);
	glClearColor(BackGround.x, BackGround.y, BackGround.z, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	processshader->use();
	processshader->setInt("HDR", 0);
	processshader->setInt("SSAO", 1);
	processshader->setInt("SSR", 2);
	processshader->setFloat("exposure", exposure);
	processshader->setBool("OpenSSAO", OpenSSAO);
	processshader->setBool("OpenSSR", OpenSSR);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, taa->historycolor);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("SSAO"));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("SSR"));
	renderQuad();
}
#endif // !DEFERED_H
