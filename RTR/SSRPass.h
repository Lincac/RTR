#pragma once

#ifndef SSRPASS_H
#define SSRPASS_H

#include"RenderHelp.h"

class SSRPass : public RenderHelp {
public:
	SSRPass();
	~SSRPass();
	virtual void RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName = "") override;
private:
	unsigned int FBO;
	unsigned int textureID;
	std::shared_ptr<Shader> shader;
	int numLevels;
};

SSRPass::SSRPass() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Window::DWWidth, Window::DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSR Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TexMap.emplace("SSR", textureID);
	shader = std::make_shared<Shader>("shader/ssr/ssr.vs", "shader/ssr/ssr.fs");
	numLevels = 1 + (int)floorf(log2f(fmaxf(Window::DWWidth, Window::DWHeight))); // 计算最大分辨率
}

SSRPass::~SSRPass() {
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &textureID);
}

void SSRPass::RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader->use();
	shader->setInt("gPosition", 0);
	shader->setInt("gNormal", 1);
	shader->setInt("gAlbedo", 2);
	shader->setInt("depthMap", 3);
	shader->setInt("gParameter", 4);
	shader->setInt("maxLevel", numLevels - 1);
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gPosition"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gNormal"));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("AlbedoRendered"));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("depthMap"));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gParameter"));
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


#endif // ! SSAOPASS_H
