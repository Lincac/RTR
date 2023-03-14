#pragma once

#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include"Light.h"

class DirectionalLight : public Light {
public:
	DirectionalLight(glm::vec3 p,glm::vec3 c) {
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

void DirectionalLight::RenderLight(glm::mat4 view,glm::mat4 projection,glm::vec3 camPos) {

	model = glm::mat4(1);
	model = glm::scale(model, glm::vec3(0.5));
	model = glm::translate(model, position);

	lightShader->use();
	lightShader->setInt("Tex", 0);

	lightShader->setMat4("model", model);
	lightShader->setMat4("view", view);
	lightShader->setMat4("projection", projection);

	lightShader->setVec3("camPos", camPos);
	lightShader->setVec3("lightPos	", position);
	lightShader->setVec3("lightCol", color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexMap.at("DirectionLight"));
	LightQuad();
}

#endif // !DIRECTIONALLIGHT_H
