#pragma once

#ifndef OBJECTS_H
#define OBJECTS_H

#include"Object.h"
class Objects : public Object{
public:
	Objects() {};

	void add(std::shared_ptr<Object> obj) { objs.push_back(obj); }
	virtual void temp_render(Shader& sha);
	virtual void render();
	virtual void updata(const int index, glm::mat4 m);
	std::vector<std::shared_ptr<Object>>  get_objs() { return objs; }


private:
	std::vector<std::shared_ptr<Object>> objs;
};

void Objects::render()  {
	for (const auto& obj : objs)
		obj->render();
}

void Objects::updata(const int index, glm::mat4 m) {
	objs[index]->updata(index, m);
}

void Objects::temp_render(Shader& sha) {
	for (const auto& obj : objs)
		obj->temp_render(sha);
}
#endif // !OBJECTS_H
