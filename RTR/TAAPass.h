#pragma once

#ifndef TAAPASS_H
#define TAAPASS_H

#include"RenderHelp.h"

class TAAPass : public RenderHelp {
public:
	TAAPass();
	virtual void RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName = "") override;
private:
	unsigned int FBO;
	unsigned int textureID;
	std::shared_ptr<Shader> shader;
};
TAAPass::TAAPass() {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	textureID = (Window::DWWidth, Window::DWHeight);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TexMap.emplace("TAA", textureID);

	//shader = std::make_shared<Shader>();
}

void TAAPass::RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName) {
	shader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	renderQuad();
}


#endif // ! CSM_H
