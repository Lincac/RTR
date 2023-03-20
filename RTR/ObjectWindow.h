#pragma once

#ifndef OBJECTWINDOW_H
#define OBJECTWINDOW_H

#include"Window.h"

class ObjectWindow : public Window {
public:
	ObjectWindow() {
	};
	virtual ~ObjectWindow() override{};
	virtual void RenderWindow(unsigned int textureID = NULL) override;
	virtual Message SendMessage() override { return Message(); };
	virtual void ReciveMessage(Message message) override {};
};

void ObjectWindow::RenderWindow(unsigned int textureID) {
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

				}
				if (ImGui::MenuItem("Sphere"))
				{

				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}


		ImGui::End();
	}
}

#endif // !OBJECTWINDOW_H
