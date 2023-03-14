#pragma once

#ifndef CUBE_H
#define CUBE_H

#include"Object.h"

class Cube : public Object {
public: 
	Cube(std::string n) {
		name = n;
		Gloss = 120;
		Albedo = TexMap.at("White");
		Normal = 0;
		Metallic = 0;
		Roughness = 0;
		Ao = 0;
		position = glm::vec3(0);
		scale = glm::vec3(1);
		rotate = glm::vec3(1);

		initVAO();
	};

	virtual void render(std::string renderModeName,std::shared_ptr<Shader> shader) override;
	virtual void GbufferRender(std::string renderModeName, std::shared_ptr<Shader> shader) override;
	virtual void temp_render(std::shared_ptr<Shader> shader) override;

	virtual glm::vec3 GetPosition() override { return position; };
	virtual glm::vec3 GetScale() override { return scale; };
	virtual glm::vec3 GetRotate() override { return rotate; };

	virtual void SetPosition(glm::vec3 pos) { position = pos; };
	virtual void SetScale(glm::vec3 sc) { scale = sc; };
	virtual void SetRotate(glm::vec3 ro) { rotate = ro; };
private: 
	std::string name;
	glm::vec3 position;
	glm::vec3 rotate;
	glm::vec3 scale;
	float Gloss;
	unsigned int VAO;
	unsigned int Albedo;
	unsigned int Normal;
	unsigned int Metallic;
	unsigned int Roughness;
	unsigned int Ao;

	void initVAO();
};

void Cube::render(std::string renderModeName,std::shared_ptr<Shader> shader)  {
	glm::mat4 model = glm::mat4(1);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(1.0f), rotate);
	model = glm::translate(model, position);

	shader->use();
	if (renderModeName == "BlinPhone")
	{
		shader->setMat4("model", model);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);

		shader->setInt("Albedo", 0);
		shader->setInt("Normal", 1);
		shader->setInt("shadowMap", 2);

		shader->setVec3("lightCol", light->GetLightCol());
		shader->setVec3("lightDir", glm::normalize(light->GetLightPos()));

		shader->setVec3("Ambient", Ambient);
		shader->setFloat("Gloss", Gloss);

		shader->setVec3("viewPos", camera.Position);
		shader->setFloat("farPlane", camera.farplane);

		shader->setInt("cascadeCount", shadowlayer.size());
		for (size_t i = 0; i < shadowlayer.size(); i++)
			shader->setFloat("cascadePlaneDistances[" + std::to_string(i) + "]", shadowlayer[i]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D_ARRAY, TexMap.at("CSM"));
	}
	else {
		shader->setInt("Albedo", 0);
		shader->setInt("Normal", 1);
		shader->setInt("Metallic", 2);
		shader->setInt("Roughness", 3);
		shader->setInt("Ao", 4);
		shader->setInt("irradianceMap", 5);
		shader->setInt("prefilterMap", 6);
		shader->setInt("LUTMap", 7);
		shader->setInt("shadowMap", 8);

		shader->setMat4("model", model);
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);

		shader->setVec3("viewPos", camera.Position);
		shader->setVec3("lightPos", light->GetLightPos());
		shader->setVec3("lightCol", light->GetLightCol());

		shader->setFloat("far_plane", camera.farplane);
		shader->setInt("countLayer", shadowlayer.size());
		for (int i = 0; i < shadowlayer.size(); i++)
			shader->setFloat("planeDistances[" + std::to_string(i) + "]", shadowlayer[i]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Metallic);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Roughness);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, Ao);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TexMap.at("irradianceMap"));
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, TexMap.at("prefilterMap"));
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, TexMap.at("LUTMap"));
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D_ARRAY, TexMap.at("shadowMap"));
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Cube::GbufferRender(std::string renderModeName, std::shared_ptr<Shader> shader) {
	glm::mat4 model = glm::mat4(1);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(1.0f), rotate);
	model = glm::translate(model, position);

	shader->use();
	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
	if (renderModeName == "BlinPhone")
	{
		shader->setInt("Albedo", 0);
		shader->setInt("Normal", 1);

		shader->setFloat("Gloss", Gloss);
		shader->setBool("OpenNormalMap", Normal == 0 ? false : true);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Normal);
	}
	else {
		shader->setInt("Albedo", 0);
		shader->setInt("Normal", 1);
		shader->setInt("Metallic", 2);
		shader->setInt("Roughness", 3);
		shader->setInt("Ao", 4);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Metallic);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Roughness);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, Ao);
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Cube::temp_render(std::shared_ptr<Shader> shader) {
	glm::mat4 model = glm::mat4(1);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(1.0f), rotate);
	model = glm::translate(model, position);

	shader->use();
	shader->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}



void Cube::initVAO() {
	float cubevertices[] = {
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), &cubevertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#endif // !CUBE_H
