#pragma once

#ifndef IRRADIANCEPASS_H
#define IRRADIANCEPASS_H

#include"RenderHelp.h"

class IrradiancePass : public RenderHelp {
public:
	IrradiancePass();
	~IrradiancePass();
	virtual void RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName = "") override{  };
private:
	unsigned int textureID;
	unsigned int captureFBO;
	unsigned int captureRBO;
	std::shared_ptr<Shader> shader;
};

IrradiancePass::IrradiancePass() {
	shader = std::make_shared<Shader>("shader/pbr/irradiance.vs", "shader/pbr/irradiance.fs");

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	shader->use();
	shader->setInt("cubeMap", 0);
	shader->setMat4("projection", capturePro);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TexMap.at("SkyBoxMap"));

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glViewport(0, 0, 32, 32);
	for (unsigned int i = 0; i < 6; i++)
	{
		shader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TexMap.emplace("irradianceMap", textureID);
}

IrradiancePass::~IrradiancePass() {
	glDeleteTextures(1, &textureID);
	glDeleteFramebuffers(1, &captureFBO);
	glDeleteRenderbuffers(1, &captureRBO);
}

#endif // !IRRADIANCEPASS_H
