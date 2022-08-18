#pragma once

#ifndef  DRAW_H
#define DRAW_H

#include"Objects.h"
#include"Cube.h"

class Draw {
public:
	Draw() {};
	Draw(std::shared_ptr<Objects>  scene)
		: objs(scene) {};

	void render() ;
	void temp_render(Shader& sha);
	void add(const char* name);
	void updata(const int index,glm::mat4 m);
	std::vector<std::shared_ptr<Object>>  get_objs() { return objs->get_objs(); }
private:
	std::shared_ptr<Objects> objs;
};
void Draw::render()  {
	objs->render();
}

void Draw::temp_render(Shader& sha) {
	objs->temp_render(sha);
}

void Draw::add(const char* name) {
	if (name == "Cube")
	{
		std::cout << "add cube" << std::endl;
		std::shared_ptr<Object> temp = std::make_shared<Cube>(std::make_shared<Shader>(GetShader("cubeshader")), glm::mat4(1.0), "cube" + std::to_string(get_objs().size()),NULL,NULL);
		objs->add(temp);
	}
}

void Draw::updata(const int index, glm::mat4 m) {
	objs->updata(index, m);
}
#endif // ! DRAW_H
