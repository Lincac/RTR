#pragma once

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include"Light.h"

class SpotLight : public Light {
public:
	SpotLight(glm::vec3 p, glm::vec3 c) {
		position = p;
		color = c;
		model = glm::mat4(1);
	};

	virtual void RenderLight(glm::mat4 view, glm::mat4 projection, glm::vec3 camPos);
	virtual glm::vec3 GetLightPos() { return position; };
	virtual glm::vec3 GetLightCol() { return color; };
	virtual void SetLightPos(glm::vec3 p) { position = p; };
	virtual void SetLightCol(glm::vec3 c) { color = c; };
private:
	glm::vec3 position;
	glm::vec3 color;
	glm::mat4 model;
};

void SpotLight::RenderLight(glm::mat4 view, glm::mat4 projection, glm::vec3 camPos) {

	model = glm::translate(model, position);

	lightShader->use();
	lightShader->setMat4("model", model);
	lightShader->setMat4("view", view);
	lightShader->setMat4("projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("LightTex"));
	LightQuad();
}
#endif // !SPOTLIGHT_H
