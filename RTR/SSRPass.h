#pragma once

#ifndef SSRPASS_H
#define SSRPASS_H

#include"RenderHelp.h"

class SSRPass : public RenderHelp {
public:
	SSRPass();
	virtual void RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName = "") override;
private:
	unsigned int FBO;
	unsigned int textureID;
	std::shared_ptr<Shader> shader;
};
SSRPass::SSRPass() {
	TexMap.emplace("SSR", textureID);

	shader = std::make_shared<Shader>("shader/ssr/ssr.vs", "shader/ssr/ssr.fs");
	shader->use();
	shader->setInt("gPosition", 0);
	shader->setInt("gNormal", 1);
	shader->setInt("gAlbedo", 2);
	shader->setInt("gParameter", 3);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	textureID = GenerateTexture2D(Window::DWWidth, Window::DWHeight);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSR Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSRPass::RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader->use();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gPosition"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gNormal"));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gAlbedo"));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gParameter"));
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


#endif // ! SSAOPASS_H
