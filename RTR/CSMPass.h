#pragma once

#ifndef CSMPASS_H
#define CSMPASS_H

#include"RenderHelp.h"	

class CSMPass : public RenderHelp
{
public:
	CSMPass ();
	~CSMPass ();
	virtual void RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName) override;
private:
	unsigned int CSMFBO;
	unsigned int CSMTexture;
	unsigned int UBO;
	std::shared_ptr<Shader> shader;
};

CSMPass ::CSMPass ()
{
	glGenFramebuffers(1, &CSMFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, CSMFBO);

	glGenTextures(1, &CSMTexture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, CSMTexture);
	glTexImage3D(  // 设置纹理格式
		GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthmapresolution, depthmapresolution, int(shadowlayer.size()) + 1,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // 设置参数，指定如何生成纹理图案
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CSMTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
		throw 0;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, nullptr, GL_STATIC_DRAW);  // 设置 glm::mat4x4*16 的缓冲内存
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);  // 将缓冲内存绑定到0上
	glBindBuffer(GL_UNIFORM_BUFFER, 0);  // 将绑定点设置为0 

	shader = std::make_shared<Shader>("shader/csm/csm.vs", "shader/csm/csm.fs", "shader/csm/csm.gs");
	TexMap.emplace("CSM", CSMTexture);
}

CSMPass ::~CSMPass ()
{
	glDeleteFramebuffers(1, &CSMFBO);
	glDeleteTextures(1, &CSMTexture);
	glDeleteBuffers(1, &UBO);
}

void CSMPass::RenderPass(std::shared_ptr<Objects> objs, std::string renderModeName = "") {
	const auto lightMat = getLightSpaceMatrices();
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	for (size_t i = 0; i < lightMat.size(); i++)
		glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMat[i]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, CSMFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_TEXTURE_2D_ARRAY, CSMTexture, 0);
	glViewport(0, 0, depthmapresolution, depthmapresolution);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);  // peter panning
	objs->temp_render(shader);
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#endif // !CSMPASS_H
