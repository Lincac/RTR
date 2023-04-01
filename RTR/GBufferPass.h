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
	unsigned int gVelo;
	unsigned int depthTexture;
	std::shared_ptr<Shader> PBRshader;
	std::shared_ptr<Shader> BlinPhoneshader;
	std::shared_ptr<Shader> MipMapShader;
};

GBufferPass::GBufferPass() {
	glGenFramebuffers(1, &gFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, DWWidth, DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, DWWidth, DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, DWWidth, DWHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	glGenTextures(1, &gParameter);
	glBindTexture(GL_TEXTURE_2D, gParameter);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, DWWidth, DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gParameter, 0);

	glGenTextures(1, &gVelo);
	glBindTexture(GL_TEXTURE_2D, gVelo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, DWWidth, DWHeight, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gVelo, 0);

	unsigned int attachment[5] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachment);

	//glGenRenderbuffers(1, &gRBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, gRBO);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, DWWidth, DWHeight);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRBO);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DWWidth, DWHeight, 0,
		GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile gbuffer" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	BlinPhoneshader = std::make_shared<Shader>("shader/gbuffer/BlinPhonegbuffer.vs", "shader/gbuffer/BlinPhonegbuffer.fs");
	PBRshader = std::make_shared<Shader>("shader/gbuffer/PBRbuffer.vs", "shader/gbuffer/PBRbuffer.fs");
	MipMapShader = std::make_shared<Shader>("shader/gbuffer/MipMap.vs", "shader/gbuffer/MipMap.fs");
	TexMap.emplace("gPosition", gPosition);
	TexMap.emplace("gNormal", gNormal);
	TexMap.emplace("gAlbedo", gAlbedo);
	TexMap.emplace("gParameter", gParameter);
	TexMap.emplace("gVelo", gVelo);
	TexMap.emplace("depthMap", depthTexture);
}

GBufferPass::~GBufferPass() {
	glDeleteFramebuffers(1, &gFBO);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedo);
	glDeleteTextures(1, &gParameter);
	glDeleteTextures(1, &gVelo);
	glDeleteRenderbuffers(1, &gRBO);
}

void GBufferPass::RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName) {
	glBindFramebuffer(GL_FRAMEBUFFER, gFBO);
	glViewport(0, 0, DWWidth, DWHeight);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(renderModeName == "PBR") objs->GbufferRender(renderModeName, PBRshader);
	else if (renderModeName == "BlinPhone") objs->GbufferRender(renderModeName,  BlinPhoneshader);

	if (OpenSSR)
	{
		// DownSample depthMap
		MipMapShader->use();
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glDepthFunc(GL_ALWAYS);
		int numLevels = 1 + (int)floorf(log2f(fmaxf(DWWidth, DWHeight))); // 计算最大分辨率
		int currentWidth = DWWidth;
		int currentHeight = DWHeight;
		for (int i = 1; i < numLevels; i++) {
			glUniform2i(glGetUniformLocation(MipMapShader->ID, "u_previousLevelDimensions"), currentWidth, currentHeight);
			MipMapShader->setInt("u_previousLevel", i - 1);
			currentWidth /= 2;
			currentHeight /= 2;
			// ensure that the viewport size is always at least 1x1
			currentWidth = currentWidth > 0 ? currentWidth : 1;
			currentHeight = currentHeight > 0 ? currentHeight : 1;
			glViewport(0, 0, currentWidth, currentHeight);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, i);
			// dummy draw command as the full screen quad is generated completely by a geometry shader
			renderQuad();
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, numLevels - 1);
		// reset the framebuffer configuration
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gParameter, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gVelo, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
		// reenable color buffer writes, reset viewport and reenable depth test
		glDepthFunc(GL_LEQUAL);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif // !GBUFFERPASS_H
