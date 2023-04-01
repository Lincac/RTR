#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include"Object.h"

class Sphere : public Object {
public:
	Sphere(std::string n, std::string renderModeName) {
		name = n;

		if (renderModeName == "PBR")
		{
			Albedo = LoadTexture("image/pbr/rusted_iron/rusted_iron_albedo.png");
			Normal = LoadTexture("image/pbr/rusted_iron/rusted_iron_normal.png");
			Metallic = LoadTexture("image/pbr/rusted_iron/rusted_iron_metallic.png");
			Roughness = LoadTexture("image/pbr/rusted_iron/rusted_iron_roughness.png");
			Ao = LoadTexture("image/pbr/rusted_iron/rusted_iron_ao.png");
		}
		else {
			Albedo = LoadTexture("image/pbr/rusted_iron/rusted_iron_albedo.png");
			Normal = 0;
		}
		Gloss = 8.0;

		indexCount = 0;
		position = glm::vec3(0);
		scale = glm::vec3(1);
		rotate = glm::vec3(1);

		preModel = glm::mat4(1);

		initVAO();
	};

	~Sphere();

	virtual void render(std::string renderModeName, std::shared_ptr<Shader> shader) override;
	virtual void GbufferRender(std::string renderModeName, std::shared_ptr<Shader> shader) override;
	virtual void temp_render(std::shared_ptr<Shader> shader) override;

	virtual std::string GetObjName() override { return name; };

	virtual glm::vec3 GetPosition() override { return position; };
	virtual glm::vec3 GetScale() override { return scale; };
	virtual glm::vec3 GetRotate() override { return rotate; };

	virtual void SetPosition(glm::vec3 pos)override { position = pos; };
	virtual void SetScale(glm::vec3 sc)override { scale = sc; };
	virtual void SetRotate(glm::vec3 ro) override { rotate = ro; };

	virtual unsigned int GetAlbedo()override { return Albedo; }
	virtual unsigned int GetNormal() override { return Normal; }
	virtual unsigned int GetMetallic()override { return Metallic; }
	virtual unsigned int GetRoughness()override { return Roughness; }
	virtual unsigned int GetAo() override { return Ao; }

	virtual void SetAlbedo(unsigned int ID) override { Albedo = ID; }
	virtual void SetNormal(unsigned int ID) override { Normal = ID; }
	virtual void SetMetallic(unsigned int ID)override { Metallic = ID; }
	virtual void SetRoughness(unsigned int ID) override { Roughness = ID; }
	virtual void SetAo(unsigned int ID) override { Ao = ID; }

	virtual float GetGloss() override { return Gloss; };
	virtual void SetGloss(float g) override { Gloss = g; };

private:
	std::string name;
	glm::vec3 position;
	glm::vec3 rotate;
	glm::vec3 scale;
	int indexCount;
	float Gloss;
	unsigned int VAO;
	unsigned int Albedo;
	unsigned int Normal;
	unsigned int Metallic;
	unsigned int Roughness;
	unsigned int Ao;

	glm::mat4 preModel;

	void initVAO();
};

Sphere::~Sphere() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &Albedo);
	glDeleteTextures(1, &Normal);
	glDeleteTextures(1, &Metallic);
	glDeleteTextures(1, &Roughness);
	glDeleteTextures(1, &Ao);
}

void Sphere::render(std::string renderModeName, std::shared_ptr<Shader> shader) {
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
		glBindTexture(GL_TEXTURE_2D_ARRAY, TexMap.at("CSM"));
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Sphere::GbufferRender(std::string renderModeName, std::shared_ptr<Shader> shader) {
	glm::mat4 model = glm::mat4(1);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(1.0f), rotate);
	model = glm::translate(model, position);

	shader->use();
	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	shader->setMat4("preProjection", preprojection);
	shader->setMat4("preView", preview);
	shader->setMat4("preModel", preModel);

	shader->setFloat("scr_width", (float)DWWidth);
	shader->setFloat("scr_height", (float)DWHeight);
	shader->setInt("offsetindex", offsetindex % 8);

	shader->setFloat("NEAR", camera.nearplane);
	shader->setFloat("FAR", camera.farplane);
	if (renderModeName == "BlinPhone")
	{
		shader->setInt("Albedo", 0);
		shader->setInt("Normal", 1);

		shader->setFloat("Gloss", Gloss);
		shader->setBool("OpenNormalMap", Normal == 0 ? false : true);
		shader->setBool("OpenSSR", false);

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
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	preModel = model;
}

void Sphere::temp_render(std::shared_ptr<Shader> shader) {
	glm::mat4 model = glm::mat4(1);
	//model = glm::scale(model, scale);
	//model = glm::rotate(model, glm::radians(1.0f), rotate);
	model = glm::translate(model, position);

	shader->use();
	shader->setMat4("model", model);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Sphere::initVAO() {
	glGenVertexArrays(1, &VAO);

	unsigned int vbo, ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	const float PI = 3.14159265359f;
	for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
	{
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			uv.push_back(glm::vec2(xSegment, ySegment));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices.push_back(y * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	indexCount = static_cast<unsigned int>(indices.size());

	std::vector<float> data;
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		if (normals.size() > 0)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}
		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	unsigned int stride = (3 + 2 + 3) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
}
#endif // !SPHERE_H
