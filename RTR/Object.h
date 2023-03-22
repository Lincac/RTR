#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include"Camera.h"
#include"DirectionalLight.h"
#include"PointLight.h"
#include"SpotLight.h"

extern const  unsigned int DWWidth;
extern const  unsigned int DWHeight;
extern const  unsigned int scr_WIDTH;
extern const  unsigned int scr_HEIGHT;

class Object {
public:
	virtual ~Object() {};
	virtual void render(std::string renderModeName, std::shared_ptr<Shader> shader)  = 0;
	virtual void GbufferRender(std::string renderModeName, std::shared_ptr<Shader> shader) = 0;
	virtual void temp_render(std::shared_ptr<Shader> shader) = 0;

	virtual std::string GetObjName() { return ""; };

	virtual glm::vec3 GetPosition() { return glm::vec3(1); };
	virtual glm::vec3 GetScale() { return glm::vec3(1); };
	virtual glm::vec3 GetRotate() { return glm::vec3(1); };
	
	virtual void SetPosition(glm::vec3 pos) {};
	virtual void SetScale(glm::vec3 sc) {};
	virtual void SetRotate(glm::vec3 ro) {};

	virtual unsigned int GetAlbedo() { return 0; }
	virtual unsigned int GetNormal() { return 0; }
	virtual unsigned int GetMetallic() { return 0; }
	virtual unsigned int GetRoughness() { return 0; }
	virtual unsigned int GetAo() { return 0; }

	virtual void SetAlbedo(unsigned int ID) {  }
	virtual void SetNormal(unsigned int ID) {  }
	virtual void SetMetallic(unsigned int ID) {  }
	virtual void SetRoughness(unsigned int ID) {  }
	virtual void SetAo(unsigned int ID) {  }

};

Camera camera(glm::vec3(0, 0, 5));
std::shared_ptr<Light> light;

glm::mat4 view;
glm::mat4 projection;

glm::mat4 preview;
glm::mat4 preprojection;

std::vector<float> shadowlayer = { camera.farplane / 50.0f,camera.farplane / 25.0f,camera.farplane / 10.0f,camera.farplane / 2.0f };

std::shared_ptr<Shader> tempshader; // Draw mesh objects
std::shared_ptr<Shader> skyboxshader; // Draw skybox
std::shared_ptr<Shader> processshader;  //  Screen processing shader
std::shared_ptr<Shader> Debugshader;

float exposure = 1.0;

bool	OpenSkyBox = false;
bool OpenHDR = false;
bool OpenSSAO = false;
bool OpenSSR = false;
bool OpenTAA = false;

int offsetindex = 0;

const unsigned int depthmapresolution = 4096;

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO;
void renderCube() {
	if (cubeVAO == 0)
	{
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
		glGenVertexArrays(1, &cubeVAO);

		glBindVertexArray(cubeVAO);
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

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

glm::mat4 capturePro = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 captureViews[] = {
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};
unsigned int LoadSkyBox(std::string path) {
	Shader tempshader("shader/skybox/hdr-cube.vs", "shader/skybox/hdr-cube.fs");

	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile captureFBO" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int cubeMap;
	glGenTextures(1, &cubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//  纹理放大和缩小时的过滤方式，当纹理坐标和图片不匹配时采用的采样方式

	unsigned int hdrtexture = loadHDRTexture("image/hdr/newport_loft.hdr");
	tempshader.use();
	tempshader.setInt("hdrskybox", 0);
	tempshader.setMat4("projection", capturePro);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrtexture);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glViewport(0, 0, 1024, 1024);
	for (unsigned int i = 0; i < 6; i++)
	{
		tempshader.setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	return cubeMap;
}

inline float lerp(float a, float b, float f) { return a + f * (b - a); };

std::vector<glm::vec4> getFrustumCornersWorldSpace(glm::mat4 pro, glm::mat4 vie) {
	const auto inv = glm::inverse(pro * vie);

	std::vector<glm::vec4> frustumCorners;
	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				const glm::vec4 pt =
					inv * glm::vec4(
						2.0f * x - 1.0f,
						2.0f * y - 1.0f,
						2.0f * z - 1.0f,
						1.0f);

				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}

glm::mat4 getLightVP(const float& near, const float& far) {

	const auto proj = glm::perspective(glm::radians(camera.Zoom), (float)DWWidth / (float)DWHeight, near, far);

	const auto centers = getFrustumCornersWorldSpace(proj, camera.GetViewMatrix());

	glm::vec3 center = glm::vec3(0.0f);
	for (const auto& v : centers)
		center += glm::vec3(v);
	center /= centers.size();

	const auto lightview = glm::lookAt(center + glm::normalize(light->GetLightPos()), center, glm::vec3(0.0f, 1.0f, 0.0f));

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::lowest();

	for (const auto& v : centers) {
		const auto& trf = lightview * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	constexpr float zMult = 10.0f;
	if (minZ < 0)
	{
		minZ *= zMult;
	}
	else
	{
		minZ /= zMult;
	}
	if (maxZ < 0)
	{
		maxZ /= zMult;
	}
	else
	{
		maxZ *= zMult;
	}

	const glm::mat4 lpMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, minZ, maxZ);
	const float scaleX = 2.0f / (maxX - minX);
	const float scaleY = 2.0f / (maxY - minY);  //  自行视口转换
	const float offsetX = -0.5f * (minX + maxX) * scaleX;
	const float offsetY = -0.5f * (minY + maxY) * scaleY;
	glm::mat4 cropMatrix(1.0f);  // 将平截头体按比例缩小并移动到
	cropMatrix[0][0] = scaleX;
	cropMatrix[1][1] = scaleY;
	cropMatrix[3][0] = offsetX;
	cropMatrix[3][1] = offsetY;
	return    cropMatrix * lpMatrix * lightview;

	//const glm::mat4 lpMatrix = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
	//return lpMatrix * lightview;
}

std::vector<glm::mat4> getLightSpaceMatrices() {
	std::vector<glm::mat4> ret;
	for (size_t i = 0; i < shadowlayer.size(); i++)
	{
		if (i == 0)
			ret.push_back(getLightVP(camera.nearplane, shadowlayer[i]));
		else if (i < shadowlayer.size())
			ret.push_back(getLightVP(shadowlayer[i - 1], shadowlayer[i]));
		else
			ret.push_back(getLightVP(shadowlayer[i - 1], camera.farplane));
	}

	return ret;
}

#endif // !OBJECT_H
