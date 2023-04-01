#pragma once

#ifndef TAAPASS_H
#define TAAPASS_H

#include"Shader.h"

class TAAPass{
public:
	TAAPass();
	~TAAPass();
	unsigned int historyframe;
	unsigned int historycolor;
	std::shared_ptr<Shader> shader;
};
TAAPass::TAAPass() {
	glGenFramebuffers(1, &historyframe);
	glBindFramebuffer(GL_FRAMEBUFFER, historyframe);

	glGenTextures(1, &historycolor);
	glBindTexture(GL_TEXTURE_2D, historycolor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DWWidth, DWHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, historycolor, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shader = std::make_shared<Shader>("shader/taa/taa.vs", "shader/taa/taa.fs");
	TexMap.emplace("AlbedoRendered", historycolor);
}

TAAPass::~TAAPass() {
	glDeleteFramebuffers(1, &historyframe);
	glDeleteTextures(1, &historycolor);
}

#endif // ! CSM_H
