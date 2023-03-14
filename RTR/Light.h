#pragma once

#ifndef LIGHT_H
#define LIGHT_H

#include<glm/glm.hpp>
#include"Shader.h"
#include"Texture.h"

#include<iostream>
#include<math.h>

class Light {
public:
	virtual void RenderLight(glm::mat4 view, glm::mat4 projection, glm::vec3 camPos) = 0;
	virtual glm::vec3 GetLightPos() = 0;
	virtual glm::vec3 GetLightCol() = 0;
	virtual void SetLightPos(glm::vec3 p) = 0;
	virtual void SetLightCol(glm::vec3 c) = 0;
};

std::shared_ptr<Shader> lightShader;
glm::vec3 Ambient;
glm::vec3 BackGround;

unsigned int LightVAO = 0;
unsigned int LightVBO;
void LightQuad() {
	if (LightVAO == 0)
	{
		float quadVertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &LightVAO);
		glGenBuffers(1, &LightVBO);
		glBindVertexArray(LightVAO);
		glBindBuffer(GL_ARRAY_BUFFER, LightVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(LightVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

#endif // !LIGHT_H
