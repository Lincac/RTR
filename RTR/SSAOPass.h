#pragma once

#ifndef SSAOPASS_H
#define SSAOPASS_H

#include"RenderHelp.h"
#include<random>

class SSAOPass : public RenderHelp {
public:
	SSAOPass();
	~SSAOPass();
	virtual void RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName = "") override;
private:
	unsigned int SSAOFBO;
	unsigned int ssaoTexture;
	unsigned int SSAOBlurFBO;
	unsigned int ssaoblurTexture;
	unsigned int noiseTexture;

	std::vector<glm::vec3> ssaoKernel; //  采样核心

	std::shared_ptr<Shader> ssaoshader;
	std::shared_ptr<Shader> ssaoblurshader;
};

SSAOPass::SSAOPass() {
	ssaoshader = std::make_shared<Shader>("shader/ssao/ssao.vs", "shader/ssao/ssao.fs");
	ssaoblurshader = std::make_shared<Shader>("shader/ssao/ssaoblur.vs", "shader/ssao/ssaoblur.fs");

	std::uniform_real_distribution<GLfloat> randomFloat(0.0f, 1.0f);
	std::default_random_engine generator;
	for (size_t i = 0; i < 64; i++)
	{
		glm::vec3 sample(  // 半球向量
			randomFloat(generator) * 2.0 - 1.0,
			randomFloat(generator) * 2.0 - 1.0,
			randomFloat(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloat(generator);
		float scale = float(i) / 64.0f;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	std::vector<glm::vec3> ssaonoise;  // 随机偏转
	for (size_t i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloat(generator) * 2.0 - 1.0,
			randomFloat(generator) * 2.0 - 1.0,
			0.0
		);
		ssaonoise.push_back(noise);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaonoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenFramebuffers(1, &SSAOFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);

	glGenTextures(1, &ssaoTexture);
	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Window::DWWidth, Window::DWHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile ssao";

	glGenFramebuffers(1, &SSAOBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFBO);

	glGenTextures(1, &ssaoblurTexture);
	glBindTexture(GL_TEXTURE_2D, ssaoblurTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Window::DWWidth, Window::DWHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoblurTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile ssaoblur";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TexMap.emplace("SSAO", ssaoblurTexture);
}

SSAOPass::~SSAOPass() {
	glDeleteFramebuffers(1, &SSAOFBO);
	glDeleteTextures(1, &ssaoTexture);
	glDeleteFramebuffers(1, &SSAOBlurFBO);
	glDeleteTextures(1, &ssaoblurTexture);
	glDeleteTextures(1, &noiseTexture);
}

void SSAOPass::RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName) {
	// ssao 
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ssaoshader->use();
	ssaoshader->setInt("gPosition", 0);
	ssaoshader->setInt("gNormal", 1);
	ssaoshader->setInt("noiseTexture", 2);

	for (size_t i = 0; i < 64; i++)
		ssaoshader->setVec3("sampler[" + std::to_string(i) + "]", ssaoKernel[i]);
	ssaoshader->setMat4("projection", projection);
	ssaoshader->setMat4("view", view);
	ssaoshader->setFloat("SCR_WIDTH", (float)Window::DWWidth);
	ssaoshader->setFloat("SCR_HEIGHT", (float)Window::DWHeight);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gPosition"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("gNormal"));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	renderQuad();

	// ssaoblur
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurFBO);
	glViewport(0, 0, Window::DWWidth, Window::DWHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ssaoblurshader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


#endif // ! SSAOPASS_H
