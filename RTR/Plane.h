#pragma once

#ifndef PLANE_H
#define PLANE_H

#include"Object.h"

class Plane :public Object {
public:
	Plane() {};
	Plane(std::shared_ptr<Shader> shader, glm::mat4 m, std::string name, unsigned int A, unsigned int N, unsigned int M,unsigned int R,unsigned int O)
		: shader(shader), model(m), VAO(0), name(name), Albedo(A), Normal(N), Metallic(M),Roughness(R), AO(O)
	{};

	virtual void render();
	virtual void temp_render(Shader& sha);
	virtual std::string get_name() const { return name; };
	virtual void updata(const int index, glm::mat4 m);
	virtual unsigned int get_textureID1() const { return Albedo; }
	virtual unsigned int get_textureID2() const { return Normal; }
	virtual  void set_textureID1(unsigned int t)  { Albedo = t; }
	virtual  void set_textureID2(unsigned int t) { Normal = t; }
	virtual glm::mat4 get_model() { return model; }

private:
	std::string name;
	std::shared_ptr<Shader> shader;
	unsigned int VAO;
	glm::mat4 model;
	unsigned int Albedo;
	unsigned int Normal;
	unsigned int Metallic;
	unsigned int Roughness;
	unsigned int AO;
};

void Plane::render() {
	if (VAO == 0)
	{
		float planeVertices[] = {
			 25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			 25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			 25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};
		GLuint planeVBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &planeVBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	shader->use();

	shader->setMat4("model", model);
	shader->setMat4("view", globalview);
	shader->setMat4("projection", globalprojection);

	shader->setInt("Albedo", 0);
	shader->setInt("Normal", 1);
	shader->setInt("Metallic", 2);
	shader->setInt("Roughness", 3);
	shader->setInt("Ao", 4);
	shader->setInt("irradianceMap", 5);
	shader->setInt("prefilterMap", 6);
	shader->setInt("LUTMap", 7);
	shader->setInt("shadowMap", 8);

	shader->setVec3("viewpos", globalcamera.Position);
	shader->setVec3("light.position", globallight.get_light_p());
	shader->setVec3("light.color", globallight.get_light_c());
	shader->setFloat("light.radius", globallight.get_light_r());
	shader->setFloat("far_plane", farplane);

	shader->setInt("countLayer", shadowlayer.size());
	for (size_t i = 0; i < shadowlayer.size(); i++)
		shader->setFloat("planeDistances[" + std::to_string(i) + "]", shadowlayer[i]);
	shader->setMat4("fs_view", globalview);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Metallic);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Roughness);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, AO);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, LUTMap);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Plane::updata(const int index, glm::mat4 m) {
	model = m;
}

void Plane::temp_render(Shader& sha) {
	if (VAO == 0)
	{
		float planeVertices[] = {
		 25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, -2.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		 25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};
		GLuint planeVBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &planeVBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	sha.use();
	sha.setInt("Albedo", 0);
	sha.setInt("Normal", 1);
	sha.setInt("Metallic", 2);
	sha.setInt("Roughness", 3);
	sha.setInt("Ao", 4);

	sha.setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Metallic);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Roughness);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, AO);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
#endif // !PLANE_H
