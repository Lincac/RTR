#pragma once

#ifndef OBJECTS_H
#define OBJECTS_H

#include"Cube.h"
#include"Sphere.h"
#include"Plane.h"

class Objects : public Object{
public:
	Objects() {};

	void add(std::shared_ptr<Object> obj) { objs.push_back(obj); }
	virtual void render(std::string renderModeName,std::shared_ptr<Shader> shader) override;
	virtual void GbufferRender(std::string renderModeName, std::shared_ptr<Shader> shader) override;
	virtual void temp_render(std::shared_ptr<Shader> shader) override;

	std::vector<std::shared_ptr<Object>>  get_objs() { return objs; }
private:
	std::vector<std::shared_ptr<Object>> objs;
};

void Objects::render(std::string renderModeName,std::shared_ptr<Shader> shader)  {
	for (const auto& obj : objs)
		obj->render(renderModeName,shader);
}

void Objects::temp_render(std::shared_ptr<Shader> shader) {
	for (const auto& obj : objs)
		obj->temp_render(shader);
}

void Objects::GbufferRender(std::string renderModeName, std::shared_ptr<Shader> shader) {
	for (const auto& obj : objs)
		obj->GbufferRender(renderModeName,shader);
};


#endif // !OBJECTS_H
