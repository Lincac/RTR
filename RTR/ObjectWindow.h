#pragma once

#ifndef OBJECTWINDOW_H
#define OBJECTWINDOW_H

#include"Window.h"

class ObjectWindow : public Window {
public:
	ObjectWindow() {};
	virtual ~ObjectWindow() override{};
	virtual void RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID = NULL) override;
};

void ObjectWindow::RenderWindow(std::shared_ptr<Objects> objs, unsigned int textureID) {
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_None);
	ImGui::SetNextWindowSize(ImVec2(float(OWWidth), float(OWHeight)), ImGuiCond_None);
	{
		ImGui::Begin("Object",NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if(ImGui::BeginMenu("Add"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					std::shared_ptr<Object> cube = std::make_shared<Cube>("Cube" + std::to_string(objs->get_objs().size()),
					renderModeName);
					objs->add(cube);
				}
				if (ImGui::MenuItem("Sphere"))
				{
					std::shared_ptr<Object> sphere = std::make_shared<Sphere>("Sphere" + std::to_string(objs->get_objs().size()), renderModeName);
					objs->add(sphere);
				}
				if (ImGui::MenuItem("Plane"))
				{
					std::shared_ptr<Object> plane = std::make_shared<Plane>("Plane" + std::to_string(objs->get_objs().size()), renderModeName);
					objs->add(plane);
				}
				if (ImGui::MenuItem("Model")) {

				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		for (const auto& temp : objs->get_objs())
		{
			if (ImGui::Button(temp->GetObjName().c_str(), ImVec2(ImGui::GetWindowWidth(), 20))) {
				ObjID = (temp->GetObjName().c_str()[temp->GetObjName().size() - 1] - '0') + 1;
			}
		}

		ImGui::End();
	}
}

#endif // !OBJECTWINDOW_H
