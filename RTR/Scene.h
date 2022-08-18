#pragma once

#ifndef SCENE_H
#define SCENE_H

#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"

#include"Draw.h"

class Scene {
public:
	Scene(GLFWwindow* w, std::shared_ptr<Draw> d):window(w),draw(d) {};
	void init();
	void render();
	void set_objs(std::vector<std::shared_ptr<Object>> o) { objs = o;}
	std::vector<std::shared_ptr<Object>> get_objs() { return objs; }

private:
	GLFWwindow* window;
	std::shared_ptr<Draw> draw;
	std::vector<std::shared_ptr<Object>> objs;
	int currentobj = 0;
	bool is_click = false;
	float scale[3] = { 1.0,1.0,1.0 };
	float position[3] = { 0.0,0.0,0.0 };
	float rotate[3] = { 0.0,1.0,0.0 };
	bool new_file = false;
	int current_texture = 0;
};

void Scene::init() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

void Scene::render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 900.0f), ImGuiCond_None);
	{
		ImGui::Begin("Scene", NULL,ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())	
		{
			if (ImGui::BeginMenu("Add"))
			{
				if (ImGui::MenuItem("Cube")) { 
					draw->add("Cube"); 
					set_objs(draw->get_objs());
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		for (int i = 0; i < objs.size(); i++)
		{
			bool ss = true;
			ImGui::Checkbox(" ", &ss);
			ImGui::SameLine();
			if (ImGui::Button(objs[i]->get_name().c_str(), ImVec2(230, 20))) {
				is_click = true;
				currentobj = i;
				scale[2] = scale[1] = scale[0] = 1.0;
				position[2] = position[1] = position[0] = 0.0;
				rotate[1]  = 1.0;
			}
		}

		ImGui::End();
	}


	ImGui::SetNextWindowPos(ImVec2(1300, 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 450.0f), ImGuiCond_None);
	{
		ImGui::Begin("object");          
		if (is_click) {
			ImGui::Text(objs[currentobj]->get_name().c_str());
			if (ImGui::DragFloat3("Scale", scale, 0.002f, -10.0f, 10.0f, "%.6f")) {
				glm::mat4 model = objs[currentobj]->get_model();   //  变换不能利用当前的，要利用前后差值
				model = glm::scale(model, glm::vec3(scale[0], scale[1], scale[2]));
				draw->updata(currentobj, model);
			}
			if (ImGui::DragFloat3("Position", position,0.002f, -10.0f, 10.0f, "%.6f")) {
				glm::mat4 model = objs[currentobj]->get_model();
				model = glm::translate(model, glm::vec3(position[0], position[1], position[2]));
				draw->updata(currentobj, model);
			}
			if (ImGui::DragFloat3("Rotate", rotate, 0.002f, -10.0f, 10.0f, "%.6f")) {
				glm::mat4 model = objs[currentobj]->get_model();
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(rotate[0], rotate[1], rotate[2]));
				draw->updata(currentobj, model);
			}
			ImGui::Image((void*)(intptr_t)objs[currentobj]->get_textureID1(), ImVec2(50, 50));
			if (ImGui::Button("change1")) {
				new_file = true;
				current_texture = 1;
			}
			ImGui::Image((void*)(intptr_t)objs[currentobj]->get_textureID2(), ImVec2(50, 50));
			if (ImGui::Button("change2")) {
				new_file = true;
				current_texture = 2;
			}
			if (new_file)
			{
				ImGui::SetNextWindowSize(ImVec2(400, 400));
				ImGui::Begin("file", &new_file, ImGuiCond_None);
				ImGui::BeginChild("Scrolling");
				for (auto iter = textures.begin(); iter != textures.end(); iter++)
				{
					if (ImGui::Selectable((iter->first).c_str()))
					{
						if (current_texture == 1)  objs[currentobj]->set_textureID1(iter->second);
						if (current_texture == 2)  objs[currentobj]->set_textureID2(iter->second);
						new_file = false;
					}
				}
				ImGui::EndChild();
				ImGui::End();
			}
		}
		ImGui::End();
	}

	ImGui::SetNextWindowPos(ImVec2(1300, 450), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 450.0f), ImGuiCond_None);
	{
		ImGui::Begin("world");                       
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif // !SCENE_H
