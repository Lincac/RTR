#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include"Global.h"

class Object {
public:
	virtual void render()  = 0;
	virtual void updata(const int index, glm::mat4 m) = 0;
	virtual void temp_render(Shader& sha) = 0;

	virtual std::string get_name() const { return NULL; };
	virtual unsigned int get_textureID1() const { return NULL; }
	virtual unsigned int get_textureID2() const { return NULL; }
	virtual  void set_textureID1(unsigned int t) {}
	virtual  void set_textureID2(unsigned int t) {}
	virtual glm::mat4 get_model() { return glm::mat4(1.0f); }

};

#endif // !OBJECT_H
