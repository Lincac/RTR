#pragma once

#ifndef GLOBAL_H
#define GLOBAL_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<random>
#include<iostream>
#include<vector>
#include<map>
#include"Camera.h"
#include"Shader.h"
#include"Light.h"

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

const GLuint SCR_WIDTH = 1600;
const GLuint SCR_HEIGHT = 900;

Camera globalcamera;
Light globallight;
const float farplane = 500.0f;
const float nearplane = 0.1f;
const unsigned int depthmapresolution = 4096;
std::vector<float> shadowlayer = { farplane / 50.0f,farplane / 25.0f,farplane / 10.0f,farplane / 2.0f };
unsigned int shadowMap;
float currentx, currenty;
unsigned int gPosition;
unsigned int gNormal;
unsigned int gAlbedo;
unsigned int gParameter;
unsigned int noiseTexture;
unsigned int ssao;
unsigned int ssaoblur;
unsigned int irradianceMap;  // 漫反射辐照度图
unsigned int prefilterMap;  // 预过滤镜面反射图
unsigned int LUTMap;

glm::mat4 globalview;
glm::mat4 globalprojection;

inline float random_float() { return rand() / (RAND_MAX + 1.0); }
inline float min(float a, float b) { return a < b ? a : b; };
inline float max(float a, float b) { return a < b ? b : a; };
inline float lerp(float a, float b, float f) { return a + f * (b - a); };

static unsigned int loadTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "error to load : " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
static unsigned int loadHDRTexture(const char* path) {
	unsigned int hdrtexture;

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		glGenTextures(1, &hdrtexture);
		glBindTexture(GL_TEXTURE_2D, hdrtexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "error to load hdrtexture" << std::endl;
	}

	return hdrtexture;
}
static unsigned int loadCubeMap(std::vector<std::string> path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	unsigned char* data = nullptr;
	for (int i = 0; i < path.size(); i++)
	{
		data = stbi_load(path[i].c_str(), &width, &height, &nrChannels, 0);  // c_str() 返回一个指向正规C字符串的 指针
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			// 目标，多极渐远纹理（远处的物体不需要高分辨率的纹理），纹理存储形式
			// 源图的格式和数据类型，图像数据
			stbi_image_free(data); // 记得释放资源
		}
		else
		{
			std::cout << "error to load " + path[i] << std::endl;
			stbi_image_free(data);
		}
	}

	// 纹理参数设置
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //  当纹理被放大或者缩小时，采用的采样方式，线性过滤
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  //  纹理焕然的方式永远只采样边界值（0，1）
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  // cube纹理有可能采样到两个面的连接处
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);// 围绕着xyz轴进行采样

	return textureID;
}

static std::map<std::string, Shader> Shaders;
static std::map<std::string, unsigned int> textures;

static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name) {
	Shaders[name] = Shader(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
};
static Shader GetShader(std::string name) {
	return Shaders[name];
};

static unsigned int LoadTexture(const char* path, std::string name) {
	textures[name] = loadTexture(path);
	return textures[name];
};
static unsigned int GetTexture(std::string name) {
	return textures[name];
};


static std::vector<glm::vec4> getFrustumCornersWorldSpace(glm::mat4 pro, glm::mat4 vie) {
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
static glm::mat4 getLightVP(const float& near, const float& far) {

	const auto proj = glm::perspective(glm::radians(globalcamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, near,far);

	const auto centers = getFrustumCornersWorldSpace(proj, globalcamera.GetViewMatrix());

	glm::vec3 center = glm::vec3(0.0f);
	for (const auto& v : centers)
		center += glm::vec3(v);
	center /= centers.size();

	const auto lightview = glm::lookAt(center + glm::normalize(globallight.get_light_p()), center, glm::vec3(0.0f, 1.0f, 0.0f));

	float minX = std::numeric_limits<float>::max();  
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();

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
static std::vector<glm::mat4> getLightSpaceMatrices() {
	std::vector<glm::mat4> ret;
	for (size_t i = 0; i < shadowlayer.size(); i++)
	{
		if (i == 0)
			ret.push_back(getLightVP(nearplane, shadowlayer[i]));
		else if (i < shadowlayer.size())
			ret.push_back(getLightVP(shadowlayer[i - 1], shadowlayer[i]));
		else
			ret.push_back(getLightVP(shadowlayer[i-1], farplane));
	}

	return ret;	
}
#endif // !GLOBAL_H
