#pragma once

#ifndef GBUFFERPASS_H
#define GBUFFERPASS_H

#include"RenderHelp.h"

class GBufferPass : public RenderHelp {
public:
	GBufferPass();
	~GBufferPass();
	virtual void RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName = "") override;
	virtual unsigned int GetPassFBO() override { return gFBO; };
private:
	unsigned int gFBO;
	unsigned int gRBO;
	unsigned int gPosition;
	unsigned int gNormal;
	unsigned int gAlbedo;
	unsigned int gParameter;
	std::shared_ptr<Shader> shader;
};

GBufferPass::GBufferPass() {
	glGenFramebuffers(1, &gFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::DWWidth, Window::DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::DWWidth, Window::DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::DWWidth, Window::DWHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	glGenTextures(1, &gParameter);
	glBindTexture(GL_TEXTURE_2D, gParameter);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::DWWidth, Window::DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gParameter, 0);

	unsigned int attachment[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachment);

	glGenRenderbuffers(1, &gRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Window::DWWidth, Window::DWHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile gbuffer" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//shader = std::make_shared<Shader>("shader/gbuffer/BlinPhonegbuffer.vs", "shader/gbuffer/BlinPhonegbuffer.fs");
	shader = std::make_shared<Shader>("shader/gbuffer/PBRbuffer.vs", "shader/gbuffer/PBRbuffer.fs");
	TexMap.emplace("gPosition", gPosition);
	TexMap.emplace("gNormal", gNormal);
	TexMap.emplace("gAlbedo", gAlbedo);
	TexMap.emplace("gParameter", gParameter);
}

GBufferPass::~GBufferPass() {
	glDeleteFramebuffers(1, &gFBO);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedo);
	glDeleteTextures(1, &gParameter);
	glDeleteRenderbuffers(1, &gRBO);
}

void GBufferPass::RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName) {
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	objs->GbufferRender(renderModeName, shader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif // !GBUFFERPASS_H
